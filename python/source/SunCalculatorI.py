import math
import mathutils as mu
import numpy as np
import csv
import sys

import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.colors as mcolors
import matplotlib.tri as tri
from matplotlib.ticker import (MultipleLocator, AutoMinorLocator)

#!pip install mathutils


mpl.rcParams['font.size'] = 7

colorMain = '#567CB5'
colorGrid = '#B0B0B0FF'
colorAxes = '#303030'

cMap = plt.cm.turbo
#https://matplotlib.org/stable/tutorials/colors/colormaps.html

colorList = ["#5f7590", "#90807c", "#c9945b", "#eaa958", "#f3c074",
             "#fcd791", "#fff2bf"]

colorList = ["#25548A", "#3C619F", "#576C9E", "#73768D", "#90807C", 
             "#AD8A6B", "#C9945B", "#E69E4A", "#EAAA58", "#EFB566", 
             "#F4C174", "#F8CC82", "#FDD791", "#FFE4A4", "#FFF3BF"]

colorList = ["#0561FF", "#056FF5", "#057CEC", "#0589E1", "#0594D5",
             "#059FC9", "#05AABA", "#05B5AC", "#05C09C", "#05CC8B",
             "#05D778", "#06E163", "#04EC4E", "#49F327", "#89F618",
             "#B4FA0A", "#DAFD06", "#FFFF03", "#FFF118", "#FFE32E",
             "#FFD437", "#FFC537", "#FFB637", "#FFA737", "#FF9837",
             "#FF8737", "#FF7537", "#FE6137", "#FC4A33", "#FD2B35", 
             "#F21D41", "#E3114D", "#D30B58", "#C00C61", "#AE0D6A"]

def makeMap(colors):   
    cMap = mcolors.LinearSegmentedColormap.from_list("", colorList)
    cMap.set_under(colorList[0])
    cMap.set_over(colorList[-1])
    return cMap

cMapL = makeMap(colorList)
cMapL_r = makeMap(colorList.reverse())

degree = math.pi/180
hour = 360*degree/24

def sampleRes(a, b, res):
    n = round((b - a)/res) + 1
    return np.linspace(a, b, n)
    
class SunCalculator:
    def __init__(self):
        self.obliquity = 23.4*degree
        self.setLatitude(0.)
        self.setLongitude(0.)
    
    def setLatitude(self, latitude):
        self.latitude = latitude
      
    def setLongitude(self, longitude):
        self.longitude = longitude
    
    def findVfromH(self, hc):
        (gamma, alpha) = hc
        cosGamma = math.cos(gamma)
        sinGamma = math.sin(gamma)
        cosAlpha = math.cos(alpha)
        sinAlpha = math.sin(alpha)
        return mu.Vector([
            sinGamma*cosAlpha,
            cosGamma*cosAlpha,
            sinAlpha
        ])
    
    def findHfromV(self, v):
        gamma = math.atan2(v.x, v.y)
        alpha = math.asin(v.z)
        if (gamma < 0.): gamma += 2.*math.pi
        return (gamma, alpha)
   
    def findVfromE(self, ec):
        (omega, delta) = ec
        cosPhi = math.cos(self.latitude)
        sinPhi = math.sin(self.latitude)
        cosOmega = math.cos(omega)
        sinOmega = math.sin(omega)
        cosDelta = math.cos(delta)
        sinDelta = math.sin(delta)
        return mu.Vector([
            -sinOmega*cosDelta,
            cosPhi*sinDelta - sinPhi*cosOmega*cosDelta,
            sinPhi*sinDelta + cosPhi*cosOmega*cosDelta
        ])
    
    def findEfromV(self, v):
        cosPhi = math.cos(self.latitude)
        sinPhi = math.sin(self.latitude)
        omega = math.atan2(-v.x, -v.y*sinPhi + v.z*cosPhi);
        delta = math.asin(v.y*cosPhi + v.z*sinPhi);
        return (omega, delta);
     
    def findEfromH(self, hc):
        return self.findEfromV(self.findVfromH(hc))
        
    def findHfromE(self, ec):
        return self.findHfromV(self.findVfromE(ec))
    
    def findLimitsE(self, delta):
        temp = -math.tan(self.latitude)*math.tan(delta)
        if temp < -1.:
            temp = -1.
        omegaS = math.acos(temp)         
        return (-omegaS, omegaS)
        
class SunNode:
    def __init__(self, v, sigma, f = 0., a = 0., w = 1., wn = 1.):
        self.v = v # vector
        self.sigma = sigma # width
        self.f = f # function
        self.a = a # amplitude
        self.w = w # weight
        self.wn = wn # weight normalize
        
    def kernel(self, r):
        arg = self.v.dot(r) - 1.
        arg /= self.sigma**2
        return math.exp(arg)
        
class SunCalculatorI(SunCalculator):
    def __init__(self):
        SunCalculator.__init__(self)
        self.nodes = []
    
    def interpolate(self, s):
        ans = 0.
        for sn in self.nodes:
            ans += sn.a*sn.kernel(s)
        return ans
    
    def sampleGridE(self, nH, nD):
        ans = []
        deltas = np.linspace(-self.obliquity, self.obliquity, nD)
        for delta in deltas:
            (omegaMin, omegaMax) = self.findLimitsE(delta)
            omegas = np.linspace(omegaMin, omegaMax, nH)
            for omega in omegas:
                ans.append([omega, delta])
        return np.array(ans)
    
    def sampleResE(self, res):
        ans = []
        deltas = sampleRes(-self.obliquity, self.obliquity, res)
        for delta in deltas:
            (omegaMin, omegaMax) = self.findLimitsE(delta)
            omegas = sampleRes(omegaMin, omegaMax, res)
            for omega in omegas:
                ans.append([omega, delta])
        return np.array(ans)
    
    def makeNodes(self, res, sigma):
        ecs = self.sampleResE(res)
        self.nodes = [SunNode(self.findVfromE(ec), sigma) for ec in ecs]           
    
    def updateAmplitudes(self):
        matrixK = np.array([[
            p.kernel(q.v) 
            for q in self.nodes]
            for p in self.nodes])
        values = [sn.v for sn in self.nodes]
        amplitudes = np.linalg.solve(matrixK, vs)   
        for (a, sn) in zip(amplitudes, self.nodes):
            sn.a = a
        
    def read(self, filename):
        file = open(filename, "r")
        reader = csv.reader(file, delimiter=',')
        
        while True:
            row = next(reader)
            if len(row) < 1: continue
            row0 = row[0]
            if row0.find('Latitude') >= 0: 
                nA = row0.find(':') + 1
                nB = row0.find('deg')
                self.latitude = float(row0[nA:nB])*degree
            elif row0.find('Longitude') >= 0: 
                nA = row0.find(':') + 1
                nB = row0.find('deg')
                self.longitude = float(row0[nA:nB])*degree
            elif row0.strip() == 'Index':
                break

        nodes = []
        while True:        
            try: row = next(reader)
            except StopIteration: break
            azimuth = float(row[1])*degree
            elevation = float(row[2])*degree
            sigma = float(row[3])*degree
            v = self.findVfromH((azimuth, elevation))
            f = float(row[4])
            a = float(row[5])
            w = float(row[6])    
            wn = float(row[7])  
            sn = SunNode(v, sigma, f, a, w, wn)
            nodes.append(sn)
        file.close()
        self.nodes = nodes
        