#ifndef FIGCONE_TREE_TREE_H
#define FIGCONE_TREE_TREE_H

#include "errors.h"
#include "streamposition.h"
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace figcone {

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
        Root,
        Any
    };

    explicit TreeNode(Type type, const StreamPosition& position = {})
        : type_{type}
        , position_{position}
    {
        switch (type_) {
        case Type::Any:
        case Type::Root:
        case Type::List:
            data_.emplace<List>();
            break;
        case Type::Item:
            data_.emplace<Item>();
            break;
        }
    }

public:
    class Item;
    class List {
    public:
        int count() const;
        const TreeNode& node(int index) const;
        TreeNode& addNode(const StreamPosition& pos = {});

    private:
        std::vector<std::unique_ptr<TreeNode>> nodeList_;
        friend class TreeNode;
    };

    class Item {
    public:
        int paramsCount() const;
        int nodesCount() const;
        bool hasParam(const std::string& name) const;
        bool hasNode(const std::string& name) const;
        const TreeParam& param(const std::string& name) const;
        const std::map<std::string, TreeParam>& params() const;
        const TreeNode& node(const std::string& name) const;
        const std::map<std::string, TreeNode>& nodes() const;

        TreeNode& addNode(const std::string& name, const StreamPosition& pos = {});
        TreeNode& addNodeList(const std::string& name, const StreamPosition& pos = {});
        TreeNode& addAny(const std::string& name, const StreamPosition& pos = {});
        void addParam(const std::string& name, const std::string& value, const StreamPosition& pos = {});
        void addParamList(
                const std::string& name,
                const std::vector<std::string>& valueList,
                const StreamPosition& pos = {});

    private:
        std::map<std::string, TreeParam> params_;
        std::map<std::string, TreeNode> nodes_;
    };

    friend class List;
    friend class Item;

public:
    bool isItem() const
    {
        if (isAny())
            return true;

        return std::holds_alternative<Item>(data_);
    }

    bool isList() const
    {
        if (isAny())
            return true;

        return std::holds_alternative<List>(data_);
    }

    const Item& asItem() const
    {
        if (isAny()){
            if (asList().count() > 1)
                throw ConfigError{"Bad any node access - trying to get multiple items as a single item", position_};
            if (asList().count() == 0){
                static const auto item = Item{};
                return item;
            }
            return asList().nodeList_.at(0)->asItem();
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
        if (isAny()){
            if (prevAccess != Type::Item){
                prevAccess = Type::Item;
                return data_.emplace<List>().addNode().asItem();
            }
            if (std::get<List>(data_).count() != 1)
                throw ConfigError{"Bad any node access - trying to get zero or multiple items as a single item"};

            prevAccess = Type::Item;
            return std::get<List>(data_).nodeList_.at(0)->asItem();
        }

        if (!isItem())
            throw ConfigError{"Bad node access - trying to get a list as a single item", position_};
        return std::get<Item>(data_);
    }

    List& asList()
    {
        if (isAny() && prevAccess != Type::List){
            prevAccess = Type::List;
            return data_.emplace<List>();
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
        return type_ == Type::Root;
    }

private:
    bool isAny() const
    {
        return type_ == Type::Any || type_ == Type::Root;
    }

private:

    std::variant<Item, List> data_;
    Type type_ = Type::Item;
    Type prevAccess = Type::Any;
    StreamPosition position_ = {1, 1};

    friend TreeNode makeTreeRoot();
};

inline int TreeNode::List::count() const
{
    return static_cast<int>(nodeList_.size());
}

inline const TreeNode& TreeNode::List::node(int index) const
{
    return *nodeList_.at(static_cast<std::size_t>(index));
}

inline TreeNode& TreeNode::List::addNode(const StreamPosition& pos)
{
    auto node = std::unique_ptr<TreeNode>{new TreeNode(TreeNode::Type::Item, pos)};
    return *nodeList_.emplace_back(std::move(node));
}

inline int TreeNode::Item::paramsCount() const
{
    return static_cast<int>(params_.size());
}

inline int TreeNode::Item::nodesCount() const
{
    return static_cast<int>(nodes_.size());
}

inline bool TreeNode::Item::hasParam(const std::string& name) const
{
    return params_.find(name) != params_.end();
}

inline bool TreeNode::Item::hasNode(const std::string& name) const
{
    return nodes_.find(name) != nodes_.end();
}

inline const TreeParam& TreeNode::Item::param(const std::string& name) const
{
    return params_.at(name);
}

inline const std::map<std::string, TreeParam>& TreeNode::Item::params() const
{
    return params_;
}

inline const TreeNode& TreeNode::Item::node(const std::string& name) const
{
    return nodes_.at(name);
}

inline const std::map<std::string, TreeNode>& TreeNode::Item::nodes() const
{
    return nodes_;
}

inline TreeNode& TreeNode::Item::addNode(const std::string& name, const StreamPosition& pos)
{
    auto node = TreeNode{Type::Item, pos};
    auto [it, ok] = nodes_.emplace(name, std::move(node));
    if (!ok)
        throw ConfigError{"Node '" + name + "' already exists", pos};

    return it->second;
}

inline TreeNode& TreeNode::Item::addNodeList(const std::string& name, const StreamPosition& pos)
{
    auto node = TreeNode{Type::List, pos};
    auto [it, ok] = nodes_.emplace(name, std::move(node));
    if (!ok)
        throw ConfigError{"Node list '" + name + "' already exists", pos};

    return it->second;
}

inline TreeNode& TreeNode::Item::addAny(const std::string& name, const StreamPosition& pos)
{
    auto node = TreeNode{Type::Any, pos};
    auto [it, ok] = nodes_.emplace(name, std::move(node));
    if (!ok)
        throw ConfigError{"Node '" + name + "' already exists", pos};

    return it->second;
}

inline void TreeNode::Item::addParam(const std::string& name, const std::string& value, const StreamPosition& pos)
{
    auto [_, ok] = params_.emplace(name, TreeParam{value, pos});
    if (!ok)
        throw ConfigError{"Parameter '" + name + "' already exists", pos};
}

inline void TreeNode::Item::addParamList(
        const std::string& name,
        const std::vector<std::string>& valueList,
        const StreamPosition& pos)
{
    auto [_, ok] = params_.emplace(name, TreeParam{valueList, pos});
    if (!ok)
        throw ConfigError{"Parameter list '" + name + "' already exists", pos};
}

inline TreeNode makeTreeRoot()
{
    return TreeNode{TreeNode::Type::Root, {1, 1}};
}

class Tree{
public:
    Tree(TreeNode&& root)
        : root_(std::move(root))
    {}

    const TreeNode& root() const
    {
        return root_;
    }

private:
    TreeNode root_;
};


} //namespace figcone

#endif //FIGCONE_TREE_TREE_H
