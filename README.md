neurons
=======

Modelisation of neurons with cliques to store/find the information in associative memories.

This is a simulation of neural netwok cliques. [This paper](http://arxiv.org/pdf/1102.4240.pdf) can offer some insight.

Neural network cliques are a new way to represent information, with efficiency comparable to binary, high tolerance to errors, and some interesting properties due to its associative nature (finding part of a message from its whole, for example). The concept was established by Vincent Gripon and Claude Berrou.

This project is a modelisation of a more complex and flexible model, nonoptimized, in order to do research. The goal of the project being to imitate the functioning of the brain, first with memory, then with basic cognitive functions.

###Description of the model

--TODO--

###Run the code

####Dependencies
```
sudo apt-get install libtbb-dev
sudo apt-get install qt5-default
sudo apt-get install qtdeclarative5-dev
```

For the python scripts:
```
sudo apt-get install ipython
sudo apt-get install python-matplotlib
```

####Build
```
qmake && make
```

The program will be put in the `bin` folder.

###License

This code is freely usable for research purposes.
