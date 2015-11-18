# [Quantum Information and Computation library (QIC_lib)](http://titas99.github.io/QIC_lib/)
Version 0.0.2 - November 18, 2015 
=================================
[QIC_lib](http://titas99.github.io/QIC_lib/) is a mordern C++11 library for general purpose quantum computing, supporting Linux, Windows and Mac OS X. 
It is a header only template library, using [Armadillo](http://arma.sourceforge.net/) (developed by Conrad Sanderson et al., Data61, Australia) for highly efficient linear algebra calculations, and if available, the [NLopt](http://ab-initio.mit.edu/wiki/index.php/NLopt) nonlinear optimization library for certain features.

Getting started
---------------
Download [QIC_lib](http://titas99.github.io/QIC_lib/) either from official [website](http://titas99.github.io/QIC_lib/) or using `git clone` with the command
    git clone https://github.com/titas99/QIC_lib.
[QIC_lib](http://titas99.github.io/QIC_lib/) is a header only library, so there is no need to compile the source.
Make sure that you have [Armadillo](http://arma.sourceforge.net/) and [NLopt](http://ab-initio.mit.edu/wiki/index.php/NLopt) installed on your system. If you don't want to use [NLopt](http://ab-initio.mit.edu/wiki/index.php/NLopt) specific features (like Quantum Discord), comment out the following line in `include/QIC_lib.hpp`:
    #define QIC_LIB_NLOPT
Include the `QIC_lib.hpp` in your source code (make sure that your compiler can find the path of the header file) and [QIC_lib](http://titas99.github.io/QIC_lib/) is ready to fly. 
For example codes, see [here](http://titas99.github.io/QIC_lib/sample.html). You will also find detailed API information [here](http://titas99.github.io/QIC_lib/documentation.html).

Got a Problem or Question?
--------------------------
If you have a question about how to use [QIC_lib](http://titas99.github.io/QIC_lib/), create a new issue at [issue tracker](https://github.com/titas99/QIC_lib/issues) labelled `discussion`.

Found an Issue or Bug?
----------------------
If you found a bug in the source code or a mistake in any kind of documentation, please let us know by adding an issue to the  [issue tracker](https://github.com/titas99/QIC_lib/issues).


You are welcomed to submit a pull request with your fix afterwards, if at hand.

Requesting a Feature?
---------------------
If you are missing some features within [QIC_lib](http://titas99.github.io/QIC_lib/), feel free to ask us about it by adding a new request to the [issue tracker](https://github.com/titas99/QIC_lib/issues) labelled `feature request`.

Note that submitting a pull request, providing the needed changes to introduced your requested feature, usually speeds up the process.

License
-------
Copyright (c) 2015 - 2016  Titas Chanda, titas DOT chanda AT gmail DOT com

[QIC_lib](http://titas99.github.io/QIC_lib/) is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

[QIC_lib](http://titas99.github.io/QIC_lib/) is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with [QIC_lib](http://titas99.github.io/QIC_lib/).  If not, see <http://www.gnu.org/licenses/>.