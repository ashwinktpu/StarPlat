import ast

class FunctionNamePrinter(ast.NodeVisitor):
    def visit_FunctionDef(self, node):
        print(f"Function name: {node.name}")
        self.generic_visit(node)



class SSSPTransformer(ast.NodeTransformer):
    def visit_
    pass


with open("SSSP.py") as file:
    tree = ast.parse(file.read())
    
print(ast.dump(tree, indent=2))
visitor = FunctionNamePrinter()
visitor.visit(tree)