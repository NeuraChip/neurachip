for file in *.tar.gz; do
    dirname="${file%.tar.gz}"
    mkdir -p "$dirname"
    tar -xzf "$file" -C "$dirname"
done
