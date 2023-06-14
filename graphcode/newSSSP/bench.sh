for f in ../../test-graphs/*; do
    touch $f-out.txt
    ./sssp $f outputs/$f-out.txt 1 | grep -oP '(?<=Total Kernel Time = :).*' 
done


