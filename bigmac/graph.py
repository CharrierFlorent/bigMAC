import io
import subprocess
import re
import time
import numpy as np
import math
from subprocess import call
from subprocess import Popen, PIPE
#call(["./bigmac", "10", "4", "0.4", "0.4"])


def test(var,dom,densite,durete):
	node = np.zeros(4, dtype=int)
	print(node)
	for i in range(0,30,1):
		proc = subprocess.Popen(["./bigmac", var, dom, densite, durete], stdout=subprocess.PIPE)
		count = 0
		for line in io.TextIOWrapper(proc.stdout, encoding="utf-8"):
			value = re.findall(r'\d+', line)[0]
			print(value)
			node[count] += int(value)
			count += 1
			print(value)
		time.sleep(1)
	for i in range (0,4,1):
		node[i] = node[i]/30
	return node[0], node[1], node[2], node[3]


durete = np.arange(0.1,1.0,0.1)
noeud_BM = np.zeros(durete.size)
noeud_BT = np.zeros(durete.size)
noeud_RFL = np.zeros(durete.size)
noeud_FC = np.zeros(durete.size)
count = 0
for d in durete:
	noeud_BTi, noeud_FCi, noeud_RFLi, noeud_BMi = test("16","8","0.4",str(d))
	noeud_BT[count] = noeud_BTi
	noeud_BM[count] = noeud_BMi
	noeud_RFL[count] = noeud_RFLi
	noeud_FC[count] = noeud_FCi
	count += 1;
print(durete)
print(noeud_BT)
print(noeud_FC)
print(noeud_RFL)
print(noeud_BM)