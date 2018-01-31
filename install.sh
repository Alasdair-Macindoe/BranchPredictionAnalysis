echo "This could take a moment"
echo "Decompressing trace files"
cd data
gunzip *
echo "Installing framework"
cd ../src
make
echo "Installing benchmarks"
cd programs
make
cd ../..
echo "Done!"
