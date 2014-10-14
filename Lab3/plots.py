#! /bin/env python

import numpy as np
import matplotlib.pyplot as plt 

def calibrate(x_range, y_range):
	plt.plot(x_range[0], y_range[0], "ro", markersize=30, label='white')
	plt.plot(x_range[1], y_range[1], "ko", markersize=30, label='black')
	plt.grid("on")
	plt.xlim(0, 2)
	plt.ylim(-1, 6)
	plt.xlabel("Encoder Color", fontsize=17)
	plt.ylabel("IR Reading (V)", fontsize=17)
	plt.title("IR Calibration Chart", fontsize=24)
	plt.legend(fontsize=20, loc='upper left')
	plt.show()

def ninety(x_range, y_range, lw):
	plt.plot(y_range, x_range[0], "r-", linewidth=lw, label='set (deg)')
	plt.plot(y_range, x_range[1], "k-", linewidth=lw, label='position (deg)')
	plt.plot(y_range, x_range[2], "b-", linewidth=lw, label='diff (deg)')
	plt.plot(y_range, x_range[3], "g-", linewidth=lw, label='control')
	plt.grid("on")
	plt.xlabel("Time (sec)", fontsize=17)
	plt.xlim(0, 10000)
	plt.title("90 Degree Step Response", fontsize=20)
	plt.legend(fontsize=15, loc='upper right')
	plt.show()

def wave(x_range, y_range, lw):
	plt.plot(y_range, x_range[0], "r-", linewidth=lw, label='set (deg)')
	plt.plot(y_range, x_range[1], "k-", linewidth=lw, label='position (deg)')
	plt.plot(y_range, x_range[2], "b-", linewidth=lw, label='diff (deg)')
	plt.plot(y_range, x_range[3], "g-", linewidth=lw, label='control')
	plt.grid("on")
	plt.xlim(0, 50000)
	plt.xlabel("Time (sec)", fontsize=17)
	plt.title("Sinewave Response", fontsize=20)
	plt.legend(fontsize=15, loc='lower right')
	plt.show()



if __name__=="__main__":

	with open("step_data2.csv") as raw_data:
		ranges = raw_data.readlines()
	parsed = list()
	for line in ranges:
		parsed.append(line.strip().split(","))
	parsed = parsed[:-1]
	as_array = np.transpose(np.asarray(parsed))
	x_range = as_array[:-1]
	y_range = as_array[-1]
	print x_range[0]
	print y_range

	wave(x_range, y_range, 2)
	#ninety(x_range, y_range, 2)
	#calibrate([.5, 1.5], [0, 5])
		
	



