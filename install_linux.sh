sudo apt-get install build-essential cmake libopenmpi-dev libopenblas-dev liblapack-dev libarmadillo-dev libmlpack-dev
sudo apt-get install octave
sudo apt-get install liboctave-dev
sudo apt-get install r-base gnuplot
sudo apt install python-dev
curl "https://bootstrap.pypa.io/pip/2.7/get-pip.py" -o get-pip.py
sudo python get-pip.py
rm get-pip.py
pip2 install numpy
python linux_build.py
