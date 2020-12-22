set -x
make;
cd ioctl
make;
cd ..

rmmod file_operations;
insmod file_operations.o

echo "Te bledy mozna zignorowac \/\n (jezeli sa)"
rm /dev/morse_tkp1;
rm /dev/morse_tkp2;
rm /dev/morse_tkp3;
rm /dev/morse_tkp4;
rm /dev/morse_tkp5;
rm /dev/morse_tkp6;
rm /dev/morse_tkp7;
rm /dev/morse_tkp8;
rm /dev/morse_tkp9;

mknod /dev/morse_tkp1 c 60 1
mknod /dev/morse_tkp2 c 60 2
mknod /dev/morse_tkp3 c 60 3
mknod /dev/morse_tkp4 c 60 4
mknod /dev/morse_tkp5 c 60 5
mknod /dev/morse_tkp6 c 60 6
mknod /dev/morse_tkp7 c 60 7
mknod /dev/morse_tkp8 c 60 8
mknod /dev/morse_tkp9 c 60 9

echo "CTRL+C == szybciej\n"
echo "a   B" > /dev/morse_tkp1
echo "aBcDeF" > /dev/morse_tkp2
echo "a-!?/*B" > /dev/morse_tkp3
echo "123" > /dev/morse_tkp4
echo "e" > /dev/morse_tkp5
echo "e" > /dev/morse_tkp6
echo "e" > /dev/morse_tkp7
echo "e" > /dev/morse_tkp8
echo "9 urzadzenie -- teraz powinno zabronic zarejestrowac\n"
echo "a   B" > /dev/morse_tkp9;


echo "sprawdzenie ioctl\n"
./ioctl/ioctl /dev/morse_tkp1 3000
echo "teraz wyswietlanie kropek powinno byc powolne"
echo a > /dev/morse_tkp1

