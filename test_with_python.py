import numpy as np
import warp_gicp


src = np.random.randn(30,3)
tgt = np.random.randn(40,3)

tfm = warp_gicp.gicp(src, tgt)
print(tfm)