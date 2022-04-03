#pragma once
#include "streamposition.h"
#include <string>
#include <vector>
#include <map>
#include <variant>

namespace figcone {

class TreeParam{
public:
    TreeParam(std::string value, const StreamPosition& position = {})
        : position_(position)
        , data_(Param{std::move(value)})
    {}

    TreeParam(std::vector<std::string> valueList, const StreamPosition& position = {})
        : position_(position)
        , data_(List{std::move(valueList)})
    {}

    struct Param{
        std::string value;
    };
    struct List{
        std::vector<std::string> valueList;
    };

    bool isParam() const
    {
        return std::holds_alternative<Param>(data_);
    }

    bool isList() const
    {
        return std::holds_alternative<List>(data_);
    }

    Param& asParam()
    {
        return std::get<Param>(data_);
    }

    List& asList()
    {
        return std::get<List>(data_);
    }

    const Param& asParam() const
    {
        return std::get<Param>(data_);
    }

    const List& asList() const
    {
        return std::get<List>(data_);
    }

    StreamPosition position() const
    {
        return position_;
    }

private:
    StreamPosition position_;
    std::variant<Param, List> data_;
};




class TreeNode{
public:
    struct Node{
        std::map<std::string, TreeParam> params;
        std::map<std::string, TreeNode> nodes;
    };
    struct List{
        std::vector<TreeNode> nodeList;
    };

    bool isNode() const
    {
        return std::holds_alternative<Node>(data_);
    }

    bool isList() const
    {
        return std::holds_alternative<List>(data_);
    }

    Node& asNode()
    {
        return std::get<Node>(data_);
    }

    List& asList()
    {
        return std::get<List>(data_);
    }

    const Node& asNode() const
    {
        return std::get<Node>(data_);
    }

    const List& asList() const
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

    std::variant<Node, List> data_;
    bool isRoot_ = false;
    StreamPosition position_{1,1};

    friend TreeNode makeTreeRoot();
    friend TreeNode makeTreeNode(const StreamPosition& pos);
    friend TreeNode makeTreeNodeList(const StreamPosition& pos);
};

inline TreeNode makeTreeRoot()
{
    auto node = TreeNode{};
    node.isRoot_ = true;
    node.position_ = StreamPosition{1,1};
    return node;
}

inline TreeNode makeTreeNode(const StreamPosition& pos = {})
{
    auto node = TreeNode{};
    node.position_ = pos;
    node.data_.emplace<TreeNode::Node>();
    return node;
}

inline TreeNode makeTreeNodeList(const StreamPosition& pos = {})
{
    auto node = TreeNode{};
    node.position_ = pos;
    node.data_.emplace<TreeNode::List>();
    return node;
}

}