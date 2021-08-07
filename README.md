# dpPred and cbPred
dpPred is a dead page predictor for the TLB and cbPred is a correlating dead block predictor for the cache that uses TLB dead page information to make decisions. For further details, please refer to our paper:

C. Mazumdar, P. Mitra and A. Basu, "Dead Page and Dead Block Predictors: Cleaning TLBs and Caches Together,"  _2021 IEEE International Symposium on High-Performance Computer Architecture (HPCA)_, 2021, pp. 507-519, doi: 10.1109/HPCA51647.2021.00050. [[Paper](https://www.csa.iisc.ac.in/~arkapravab/papers/hpca21_DOA.pdf)]  [[Video](https://www.csa.iisc.ac.in/~arkapravab/papers/HPCA21_DeadPage_Talk_video_long.mp4)]

**Execution instructions**

 - Building Sniper: `make` from base directory
 - Executing applications: `./run-sniper -c run <application>` after compilation. For instance, to run "ls -l" on the simulator, `./run-sniper -c run "ls -l"`

The work was implemented on top Snipersim, the README of which is kept intact below:

# README
This is the source code for the Sniper multicore simulator developed
by the Performance Lab research group at Ghent University, Belgium.
Please refer to the NOTICE file in the top level directory for
licensing and copyright information.

For the latest version of the software or additional information, please
see our website:

[http://snipersim.org](http://snipersim.org/)

If you are using Sniper, please let us know by posting a message on
our user forum.  If you use Sniper 6.0 or later in your research,
(if you are using the Instruction-Window Centric core model, etc.),
please acknowledge us by referencing our TACO 2014 paper:

Trevor E. Carlson, Wim Heirman, Stijn Eyerman, Ibrahim Hur, Lieven
Eeckhout, "An Evaluation of High-Level Mechanistic Core Models".
In ACM Transactions on Architecture and Code Optimization (TACO),
Volume 11, Issue 3, October 2014, Article No. 28
[http://dx.doi.org/10.1145/2629677](http://dx.doi.org/10.1145/2629677)

If you are using earlier versions of Sniper, please acknowledge
us by referencing our SuperComputing 2011 paper:

Trevor E. Carlson, Wim Heirman, Lieven Eeckhout, "Sniper: Exploring
the Level of Abstraction for Scalable and Accurate Parallel Multi-Core
Simulation". Proceedings of the International Conference for High
Performance Computing, Networking, Storage and Analysis (SC),
pages 52:1--52:12, November 2011.
[http://dx.doi.org/10.1145/2063384.2063454](http://dx.doi.org/10.1145/2063384.2063454)
