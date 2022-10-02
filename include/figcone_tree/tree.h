#ifndef FIGCONE_TREE_TREE_H
#define FIGCONE_TREE_TREE_H

#include "streamposition.h"
#include "errors.h"
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <variant>

namespace figcone {

class TreeParam {
    struct Item {
        std::string value;
    };
    struct List {
        std::vector<std::string> valueList;
    };

public:
    TreeParam(std::string value, const StreamPosition& position = {})
        : position_(position)
        , data_(Item{std::move(value)})
    {}

    TreeParam(std::vector <std::string> valueList, const StreamPosition& position = {})
        : position_(position)
        , data_(List{std::move(valueList)})
    {}

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
        return std::get<Item>(data_).value;
    }

    const std::vector<std::string> valueList() const
    {
        return std::get<List>(data_).valueList;
    }


    StreamPosition position() const
    {
        return position_;
    }

private:
    StreamPosition position_;
    std::variant <Item, List> data_;
};

class TreeNode {
public:
    class List {
    public:
        int count() const
        {
            return static_cast<int>(nodeList_.size());
        }

        const TreeNode& node(int index) const
        {
            return nodeList_.at(static_cast<std::size_t>(index));
        }

        TreeNode& addNode(const StreamPosition& pos = {})
        {
            auto node = TreeNode{};
            node.position_ = pos;
            node.data_.emplace<TreeNode::Item>();
            return nodeList_.emplace_back(std::move(node));
        }

    private:
        std::deque<TreeNode> nodeList_;
    };

    class Item {
    public:
        int paramsCount() const
        {
            return static_cast<int>(params_.size());
        }

        int nodesCount() const
        {
            return static_cast<int>(nodes_.size());
        }

        bool hasParam(const std::string& name) const
        {
            return params_.find(name) != params_.end();
        }

        bool hasNode(const std::string& name) const
        {
            return nodes_.find(name) != nodes_.end();
        }

        const TreeParam& param(const std::string& name) const
        {
            return params_.at(name);
        }

        const std::map<std::string, TreeParam>& params() const
        {
            return params_;
        }

        const TreeNode& node(const std::string& name) const
        {
            return nodes_.at(name);
        }

        const std::map<std::string, TreeNode>& nodes() const
        {
            return nodes_;
        }

        TreeNode& addNode(const std::string& name, const StreamPosition& pos = {})
        {
            auto node = TreeNode{};
            node.position_ = pos;
            node.data_.emplace<Item>();
            auto [it, ok] = nodes_.emplace(name, std::move(node));
            if (!ok)
                throw ConfigError{"Node '" + name + "' already exists", pos};

            return it->second;
        }

        TreeNode& addNodeList(const std::string& name, const StreamPosition& pos = {})
        {
            auto node = TreeNode{};
            node.position_ = pos;
            node.data_.emplace<List>();
            auto [it, ok] = nodes_.emplace(name, std::move(node));
            if (!ok)
                throw ConfigError{"Node list '" + name + "' already exists", pos};

            return it->second;
        }

        void addParam(const std::string& name, const std::string& value, const StreamPosition& pos = {})
        {
            auto [_, ok] = params_.emplace(name, TreeParam{value, pos});
            if (!ok)
                throw ConfigError{"Parameter '" + name + "' already exists", pos};
        }

        void addParamList(const std::string& name, const std::vector <std::string>& valueList,
                          const StreamPosition& pos = {})
        {
            auto [_, ok] = params_.emplace(name, TreeParam{valueList, pos});
            if (!ok)
                throw ConfigError{"Parameter list '" + name + "' already exists", pos};
        }

    private:
        std::map<std::string, TreeParam> params_;
        std::map<std::string, TreeNode> nodes_;
    };

    bool isItem() const
    {
        return std::holds_alternative<Item>(data_);
    }

    bool isList() const
    {
        return std::holds_alternative<List>(data_);
    }

    const Item& asItem() const
    {
        return std::get<Item>(data_);
    }

    const List& asList() const
    {
        return std::get<List>(data_);
    }

    Item& asItem()
    {
        return std::get<Item>(data_);
    }

    List& asList()
    {
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
    TreeNode() = default;
    std::variant<Item, List> data_;
    bool isRoot_ = false;
    StreamPosition position_{1, 1};

    friend TreeNode makeTreeRoot();
};

inline TreeNode makeTreeRoot()
{
    auto root = TreeNode{};
    root.isRoot_ = true;
    return root;
}

}

#endif //FIGCONE_TREE_TREE_H
