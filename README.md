# LartC - Compiler for Lart Programming Language

Example from [lart-examples](github.com:frefolli/lart-examples):

```tree.lart
typedef u64 = integer<64, false>;

fn malloc(size: u64) -> &void;
fn free(ptr: &void);

typedef Node = struct {
  left: &Node,
  right: &Node
};

typedef Tree = struct {
  root: &Node
};

fn new_node(left: &Node, right: &Node) -> &Node {
  let node: &Node = cast<&Node>(malloc(sizeof<Node>));
  node->left = left;
  node->right = right;
  return node;
}

fn new_tree(root: &Node) -> &Tree {
  let tree: &Tree = cast<&Tree>(malloc(sizeof<Tree>));
  tree->root = root;
  return tree;
}

fn delete_node(node: &Node) {
  if (node != nullptr) {
    delete_node(node->left);
    delete_node(node->right);
    free(cast<&void>(node));
  }
}

fn delete_tree(tree: &Tree) {
  if (tree != nullptr) {
    delete_node(tree->root);
    free(cast<&void>(tree));
  }
}

fn main() {
  let tree: &Tree = new_tree(
    new_node(
      new_node(
        new_node(nullptr, nullptr),
        new_node(nullptr, nullptr)
        ),
      new_node(
        new_node(nullptr, nullptr),
        new_node(nullptr, nullptr)
        )
    )
  );
  delete_tree(tree);
}
```
