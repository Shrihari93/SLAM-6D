Strategy
========

#### At each level
* Block matching. Reduce lateral shift, more transverse shift. [Try stretching FORE image along transverse direction for better matching (?).]
* Outlier removal. Remove using lateral shift. Now filter away all points not close to modal value in [25x25(?)] block.
* Interpolate. Use neighbouring 8 points as well.
* Go to next stage. Use previous stage disparity values [and stretch values if possible].


### Revised strategy:
* 