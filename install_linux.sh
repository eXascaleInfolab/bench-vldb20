sudo apt-get install -y build-essential cmake libopenmpi-dev libopenblas-dev liblapack-dev libarmadillo-dev libmlpack-dev
sudo apt-get install -y octave
sudo apt-get install -y liboctave-dev
sudo apt-get install -y r-base gnuplot
sudo apt-get install -y python2-dev
curl "https://bootstrap.pypa.io/pip/2.7/get-pip.py" -o get-pip.py
sudo python2 get-pip.py
rm get-pip.py
pip2 install numpy
python2 linux_build.py
