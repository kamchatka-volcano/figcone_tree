#ifndef FIGCONE_TREE_TREE_H
#define FIGCONE_TREE_TREE_H

#include "errors.h"
#include "streamposition.h"
#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace figcone {

namespace detail{

template<typename T>
std::vector<std::string> keys(const std::map<std::string, T>& map)
{
    auto result = std::vector<std::string>{};
    result.reserve(map.size());
    std::transform(map.begin(), map.end(), std::back_inserter(result), [](const auto& pair){return pair.first;});
    return result;
}

}

class TreeParam {
    struct Item {
        std::string value;
    };
    struct List {
        std::vector<std::string> valueList;
    };

public:
    explicit TreeParam(std::string value, const StreamPosition& position = {})
        : position_(position)
        , data_(Item{std::move(value)})
    {
    }

    explicit TreeParam(std::vector<std::string> valueList, const StreamPosition& position = {})
        : position_(position)
        , data_(List{std::move(valueList)})
    {
    }

    bool isItem() const
    {
        return std::holds_alternative<Item>(data_);
    }

    bool isList() const
    {
        return std::holds_alternative<List>(data_);
    }

    const std::string& value() const
    {
        if (!isItem())
            throw ConfigError{"Bad parameter access - trying to get a list as a single item", position_};

        return std::get<Item>(data_).value;
    }

    const std::vector<std::string>& valueList() const
    {
        if (!isList())
            throw ConfigError{"Bad parameter access - trying to get a single item as a list", position_};

        return std::get<List>(data_).valueList;
    }

    StreamPosition position() const
    {
        return position_;
    }

private:
    StreamPosition position_;
    std::variant<Item, List> data_;
};

class TreeNode {
    enum class Type {
        Item,
        List,
        Any
    };

    explicit TreeNode(Type type, std::string name = {}, const StreamPosition& position = {})
        : data_{List()}
        , type_{type}
        , name_{std::move(name)}
        , position_{position}
    {
        switch (type_) {
        case Type::Any:
        case Type::List:
            data_.emplace<List>();
            break;
        case Type::Item:
            data_.emplace<Item>(nullptr);
            break;
        }
    }

public:
    class List {
    public:
        int size() const
        {
            return static_cast<int>(nodeList_.size());
        }

        const TreeNode& at(int index) const
        {
            return *nodeList_.at(static_cast<std::size_t>(index));
        }

        TreeNode& emplaceBack(std::string_view name, const StreamPosition& pos = {})
        {
            auto node = std::unique_ptr<TreeNode>{new TreeNode(TreeNode::Type::Item, std::string{name}, pos)};
            return *nodeList_.emplace_back(std::move(node));
        }

        TreeNode& emplaceBack(const StreamPosition& pos = {})
        {
            auto node = std::unique_ptr<TreeNode>{new TreeNode(TreeNode::Type::Item, {}, pos)};
            return *nodeList_.emplace_back(std::move(node));
        }

        TreeNode& emplaceBackAny(std::string_view name, const StreamPosition& pos = {})
        {
            auto node = std::unique_ptr<TreeNode>{new TreeNode(TreeNode::Type::Any, std::string{name}, pos)};
            return *nodeList_.emplace_back(std::move(node));
        }

        TreeNode& emplaceBackAny(const StreamPosition& pos = {})
        {
            auto node = std::unique_ptr<TreeNode>{new TreeNode(TreeNode::Type::Any, {}, pos)};
            return *nodeList_.emplace_back(std::move(node));
        }

    private:
        std::vector<std::unique_ptr<TreeNode>> nodeList_;
        friend class TreeNode;
    };

    class Item {
    public:
        Item(std::vector<std::unique_ptr<TreeNode>>* nodeList = nullptr)
            : nodeList_{nodeList}
        {
        }

        int paramsCount(bool checkIfInvokedThroughAdapterItem = true) const
        {
            if (checkIfInvokedThroughAdapterItem && nodeList_ && !nodeList_->empty())
                return nodeList_->at(0)->asItem().paramsCount(false);

            return static_cast<int>(params_.size());
        }

        int nodesCount(bool checkIfInvokedThroughAdapterItem = true) const
        {
            if (checkIfInvokedThroughAdapterItem && nodeList_ && !nodeList_->empty())
                return nodeList_->at(0)->asItem().nodesCount(false);

            return static_cast<int>(nodes_.size());
        }

        bool hasParam(const std::string& name, bool checkIfInvokedThroughAdapterItem = true) const
        {
            if (checkIfInvokedThroughAdapterItem && nodeList_ && !nodeList_->empty())
                return nodeList_->at(0)->asItem().hasParam(name, false);

            return params_.find(name) != params_.end();
        }

        bool hasNode(const std::string& name, bool checkIfInvokedThroughAdapterItem = true) const
        {
            if (checkIfInvokedThroughAdapterItem && nodeList_ && !nodeList_->empty()) {
                if (nodeList_->at(0)->name_ == name)
                    return true;
                return nodeList_->at(0)->asItem().hasNode(name, false);
            }

            return nodes_.find(name) != nodes_.end();
        }

        const TreeParam& param(const std::string& name, bool checkIfInvokedThroughAdapterItem = true) const
        {
            if (checkIfInvokedThroughAdapterItem && nodeList_ && !nodeList_->empty())
                return nodeList_->at(0)->asItem().param(name, false);

            return params_.at(name);
        }

        std::vector<std::string> paramNames() const
        {
            if (nodeList_ && !nodeList_->empty())
                return detail::keys(nodeList_->at(0)->asItem().params_);

            return detail::keys(params_);
        }

        const TreeNode& node(const std::string& name, bool checkIfInvokedThroughAdapterItem = true) const
        {
            if (checkIfInvokedThroughAdapterItem && nodeList_ && !nodeList_->empty()) {
                if (nodeList_->at(0)->name_ == name)
                    return *nodeList_->at(0);

                return nodeList_->at(0)->asItem().node(name, false);
            }

            return nodes_.at(name);
        }

        std::vector<std::string> nodeNames() const
        {
            if (nodeList_ && !nodeList_->empty()) {
                if (nodeList_->size() == 1)
                    return {nodeList_->at(0)->name_};

                return detail::keys(nodeList_->at(0)->asItem().nodes_);
            }

            return detail::keys(nodes_);
        }

        TreeNode& addNode(const std::string& name, const StreamPosition& pos = {}, bool checkIfInvokedThroughAdapterItem = true)
        {
            if (checkIfInvokedThroughAdapterItem && nodeList_ && !nodeList_->empty())
                return nodeList_->at(0)->asItem().addNode(name, pos, false);

            auto node = TreeNode{Type::Item, name, pos};
            auto [it, ok] = nodes_.emplace(name, std::move(node));
            if (!ok)
                throw ConfigError{"Node '" + name + "' already exists", pos};

            return it->second;
        }

        TreeNode& addNodeList(const std::string& name, const StreamPosition& pos = {}, bool checkIfInvokedThroughAdapterItem = true)
        {
            if (checkIfInvokedThroughAdapterItem && nodeList_ && !nodeList_->empty())
                return nodeList_->at(0)->asItem().addNodeList(name, pos, false);

            auto node = TreeNode{Type::List, name, pos};
            auto [it, ok] = nodes_.emplace(name, std::move(node));
            if (!ok)
                throw ConfigError{"Node list '" + name + "' already exists", pos};

            return it->second;
        }

        TreeNode& addAny(const std::string& name, const StreamPosition& pos = {}, bool checkIfInvokedThroughAdapterItem = true)
        {
            if (checkIfInvokedThroughAdapterItem && nodeList_ && !nodeList_->empty())
                return nodeList_->at(0)->asItem().addAny(name, pos, false);

            auto node = TreeNode{Type::Any, name, pos};
            auto [it, ok] = nodes_.emplace(name, std::move(node));
            if (!ok)
                throw ConfigError{"Node '" + name + "' already exists", pos};

            return it->second;
        }

        void addParam(const std::string& name, const std::string& value, const StreamPosition& pos = {}, bool checkIfInvokedThroughAdapterItem = true)
        {
            if (checkIfInvokedThroughAdapterItem && nodeList_ && !nodeList_->empty()) {
                return nodeList_->at(0)->asItem().addParam(name, value, pos, false);
            }

            auto [_, ok] = params_.emplace(name, TreeParam{value, pos});
            if (!ok)
                throw ConfigError{"Parameter '" + name + "' already exists", pos};
        }

        void addParamList(
                const std::string& name,
                const std::vector<std::string>& valueList,
                const StreamPosition& pos = {}, bool checkIfInvokedThroughAdapterItem = true)
        {
            if (checkIfInvokedThroughAdapterItem && nodeList_ && !nodeList_->empty())
                return nodeList_->at(0)->asItem().addParamList(name, valueList, pos, checkIfInvokedThroughAdapterItem);

            auto [_, ok] = params_.emplace(name, TreeParam{valueList, pos});
            if (!ok)
                throw ConfigError{"Parameter list '" + name + "' already exists", pos};
        }

    private:
        std::map<std::string, TreeParam> params_;
        std::map<std::string, TreeNode> nodes_;
        std::vector<std::unique_ptr<TreeNode>>* nodeList_ = nullptr;
    };

public:
    bool isItem() const
    {
        if (type_ == Type::Any)
            return true;

        return std::holds_alternative<Item>(data_);
    }

    bool isList() const
    {
        if (type_ == Type::Any)
            return true;

        return std::holds_alternative<List>(data_);
    }

    const Item& asItem() const
    {
        if (type_ == Type::Any) {
            if (asList().size() > 1)
                throw ConfigError{"Bad any node access - trying to get multiple items as a single item", position_};
            return listAdapterItem_;
        }

        if (!isItem())
            throw ConfigError{"Bad node access - trying to get a list as a single item", position_};

        return std::get<Item>(data_);
    }

    const List& asList() const
    {
        if (!isList())
            throw ConfigError{"Bad node access - trying to get a single item as a list", position_};

        return std::get<List>(data_);
    }

    Item& asItem()
    {
        if (type_ == Type::Any) {
            if (prevAccess != Type::Item) {
                prevAccess = Type::Item;
                auto& list = data_.emplace<List>();
                listAdapterItem_ = Item{&list.nodeList_};
                list.emplaceBack(name_);
                return listAdapterItem_;
            }
            if (std::get<List>(data_).size() != 1)
                throw ConfigError{
                        "Bad any node access - trying to get zero or multiple items as a single item",
                        position_};

            prevAccess = Type::Item;
            return listAdapterItem_;
        }

        if (!isItem())
            throw ConfigError{"Bad node access - trying to get a list as a single item", position_};
        return std::get<Item>(data_);
    }

    List& asList()
    {
        if (type_ == Type::Any && prevAccess != Type::List) {
            prevAccess = Type::List;
            auto& list = data_.emplace<List>();
            listAdapterItem_ = Item{&list.nodeList_};
            return list;
        }

        if (!isList())
            throw ConfigError{"Bad node access - trying to get a single item as a list", position_};
        return std::get<List>(data_);
    }

    const StreamPosition& position() const
    {
        return position_;
    }

    bool isRoot() const
    {
        return isRoot_;
    }

private:
    std::variant<Item, List> data_;
    Type type_ = Type::Item;
    Type prevAccess = Type::Any;
    std::string name_;
    StreamPosition position_ = {1, 1};
    Item listAdapterItem_;
    bool isRoot_ = false;

    friend std::unique_ptr<TreeNode> makeTreeRoot();
    friend std::unique_ptr<TreeNode> makeTreeRootList();
};

inline std::unique_ptr<TreeNode> makeTreeRoot()
{
    auto root = std::unique_ptr<TreeNode>{new TreeNode(TreeNode::Type::Item, "root", {1, 1})};
    root->isRoot_ = true;
    return root;
}

inline std::unique_ptr<TreeNode> makeTreeRootList()
{
    auto root = std::unique_ptr<TreeNode>{new TreeNode(TreeNode::Type::List, "root", {1, 1})};
    root->isRoot_ = true;
    return root;
}

class Tree {
public:
    Tree(std::unique_ptr<TreeNode> root)
        : root_(std::move(root))
    {
    }

    const TreeNode& root() const
    {
        return *root_;
    }

private:
    std::unique_ptr<TreeNode> root_;
};

} //namespace figcone

#endif //FIGCONE_TREE_TREE_H
