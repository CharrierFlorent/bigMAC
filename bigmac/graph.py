import io
import subprocess
import re
import time
import numpy as np
import math
import matplotlib.pyplot as plt
from multiprocessing import Process
import time

from subprocess import call
from subprocess import Popen, PIPE
#call(["./bigmac", "10", "4", "0.4", "0.4"])

def draw(x,y1,y2,y3,y4, xlabel, ylabel, filename):
	plt.plot(x,y1)
	plt.plot(x,y2)
	plt.plot(x,y3)
	plt.plot(x,y4)
	plt.legend(['BT', 'FC', 'RFL', 'BM'], loc='upper left')
	plt.xlabel(xlabel)
	plt.ylabel(ylabel)
	#plt.show()
	plt.savefig('plot/'+filename+'.png')
	plt.close()

def test(var,dom,densite,durete):
	node = np.zeros(4, dtype=int)
	time = np.zeros(4, dtype=int)
	
	for i in range(0,30,1):
		proc = subprocess.Popen(["./bigmac", var, dom, densite, durete], stdout=subprocess.PIPE)
		count = 0
		cpt = 1
		for line in io.TextIOWrapper(proc.stdout, encoding="utf-8"):
			value = re.findall(r'\d+', line)[0]
			if count%2 == 0:
				node[int(count/2)] += int(value)
			else:
				time[count-1*cpt] += int(value)
				cpt  += 1
			count += 1
	for i in range (0,4,1):
		node[i] = node[i]/30
		time[i] = time[i]/30
	return node,time

def expe(nb_var, dom, densite):
	durete = np.arange(0.1,1.0,0.1)
	noeud_BM = np.zeros(durete.size)
	noeud_BT = np.zeros(durete.size)
	noeud_RFL = np.zeros(durete.size)
	noeud_FC = np.zeros(durete.size)
	time_BM = np.zeros(durete.size)
	time_BT = np.zeros(durete.size)
	time_RFL = np.zeros(durete.size)
	time_FC = np.zeros(durete.size)
	count = 0
	for d in durete:
		node, time = test(nb_var,dom,densite,str(d))
		noeud_BT[count] = node[0]
		noeud_BM[count] = node[1]
		noeud_RFL[count] = node[2]
		noeud_FC[count] = node[3]
		time_BT[count] = time[0]
		time_BM[count] = time[1]
		time_RFL[count] = time[2]
		time_FC[count] = time[3]
		count += 1;
	draw(durete,noeud_BT,noeud_FC,noeud_RFL,noeud_BM,'durete', 'noeuds explores','node'+'-'+str(nb_var)+'-'+str(dom)+'-'+str(densite).split('.')[1])
	draw(durete,time_BT,time_FC,time_RFL,time_BM,'durete', 'temps cpu (µs)','time'+'-'+str(nb_var)+'-'+str(dom)+'-'+str(densite).split('.')[1])


var = ["6","8","16","32","64"]
dom = ["4","10","6","12","24"]
densite = ["0.2","0.4","0.6","0.8"]
for v in var:
	for d in dom:
		for den in densite:
			print("Pour "+v+" var, taille domaine " + d + " et densite " + den)
			action_process = Process(target=expe, args=(v,d,den))
			action_process.start()
			action_process.join(timeout=300)
			action_process.terminate()
