import os
import sys
import subprocess

input_data_dir = "../../test-graphs"
program = "./sssp"

def main():
    try:
        os.mkdir("outputs")
    except FileExistsError:
        pass

    for inp_file in os.listdir(input_data_dir):
        graph_name = inp_file[:-4] # remove the ".txt"

        # create output file
        open(f"outputs/{graph_name}", "w")

        result = subprocess.run([program,
                                 os.path.join(input_data_dir, inp_file),
                                 "1"],
                                capture_output=True, text=True)
        print(graph_name)
        print(result.stdout)


if __name__ == "__main__":
    main()

