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

    with open("results.txt", "w") as res:
        for inp_file in os.listdir(input_data_dir):
            graph_name = inp_file[:-4] # remove the ".txt"

            print(f"benchmarking {program} on {graph_name}")

            src = 1
            if graph_name == "clean-soc-twitter":
                src = 19537227
            elif graph_name == "clean-soc-sinaweibo":
                src = 255661

            print(f"choosing {src} as source")

            print(f"input file is {inp_file}")

            out_file = os.path.join("outputs", f"{graph_name}-out.txt")
            print(f"creating output file {out_file}")
            # create output file
            open(out_file, "w")

            print("running program")
            result = subprocess.run([program,
                                     os.path.join(input_data_dir, inp_file),
                                     out_file,
                                     str(src)],
                                    capture_output=True, text=True)

            print(f"return code: {result.returncode}")
            #print(result.stdout)
            #print(result.stderr)

            print("writing results to file")
            for l in result.stdout.splitlines():
                if not l.startswith("Total Kernel time"):
                    continue
                t = float(l.split("=")[1].strip().split()[0])
                res.write(f"{graph_name}: {t}\n")
                print(f"kertime = {t}")

            print("done")
            print()


if __name__ == "__main__":
    main()

