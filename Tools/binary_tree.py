# Helper function to create the binary tree structure needed for Day 18 of 2021

def binary_tree(depth_size=4):
    tree = []
    
    def add_node(node, depth=0, max_depth=depth_size):
        my_depth = depth + 1
        if my_depth > max_depth:
            return
        
        if len(node) == 0:
            node.extend([[],[]])
        
        for subnode in node:
            add_node(subnode, depth=my_depth)
    
    add_node(tree)
    
    return tree