# checkNodeCoverage
The program is designed to check the coverage of an overlying tree node by a set of nodes. A tree is a connected graph without cycles. An overlying node is any node that is on the path from the current node up to the root. A tree node is considered covered if at least one node is selected on each path from this node to any leaf.
 
## Functional requirements

*Actions on objects*
 
The program should check whether the set of nodes specified by the user covers the selected node of the tree.
The program should determine the minimum list of nodes that need to be added to complete the coverage, if it is incomplete.
If a node that is not a descendant of the target node is selected, the program should output the message "Node [name] is not a descendant of the target node."
If a node is marked that is a descendant of the marked one, the program should output the message "Node [name] is redundant (located in a subtree of the already marked node [name])".

 
## Restrictions
 
The input graph must be oriented and contain no cycles (have a tree structure).

## Input and output data
 
The program should receive two command-line parameters: the name of the input file with a description of the tree in the DOT language and the names of 2 files for writing the result (a file with a modified description of the graph and a file with a report on the program).
Example of a program launch command: app.exe C:\\Documents\input.dot .\out.dot.\out2.txt .
The input data is presented as a single text file with the .dot extension. It contains a description of the graph in the DOT language. The file should contain only the basic structure of the directed graph. Any extended DOT functions (subgraphs, clusters, line styles, labels, etc.) are prohibited. The graph described in the input file must be oriented (digraph in DOT language). That is, the entire description must be enclosed in the digraph block G { ... }. The target node must have a diamond shape and must be strictly one. The marked nodes that form the coverage set must have a rectangular shape. Any other attributes (color, text, line style, etc.) are prohibited in the input file. The relationships between nodes are described strictly in the format [PARENT] -> [DESCENDANT]; or [PARENT] -> [DESCENDANT 1] -> [DESCENDANT 2] -> [DESCENDANT N];.
Sample input file:


*digraph G {
   first[shape = diamond];
   second[shape = rectangle];  
   third[shape = rectangle];  
   first -> second;
   first -> third;
}*


The output file with the program report must be a text file with the .txt extension. This file contains a text report: "Target node IS COVERED" or "Target node is NOT COVERED". As well as a list of nodes that need to be added if the coverage is incomplete.
(The target node IS NOT COVERED.
Missing nodes: [node 1], [node 2], etc.)
If a node that is not a descendant of the target is marked, or a node that is a descendant of the marked node is marked, information about these errors is displayed in a separate block with an error description.
The output file for the modified graph must be a text file with the .dot extension. In this file, the original structure of the tree is complemented by a color indication: the nodes that correctly form the cover are colored green, the elements mistakenly selected by the user are red, and the missing nodes proposed by the program are orange. If critical errors occur that make tree construction impossible, the output file is not generated.

