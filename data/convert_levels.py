# This file converts levels from json format into a format more suitable for the Pi.

from struct import pack,unpack
import json

def conv_lev(name='lev0'):
    with open(name+'.json','rb') as fd:
        A=json.load(fd)
        L,P=A
        w=len(L)
        h=len(L[0])
        with open(name+'.pi','wb') as out:
            out.write(pack('iii',w,h,len(P)))
            for a in L:
                for [b,c,h,dx,dy] in a:
                    out.write(pack('iifff',b,0,h,dx,dy)) # Note that we overwrite the contents to zero here
            for p in P:
                out.write(pack('ii',0,0))
                for a in 'type state thetaf square bounce numfish size speed steps'.split():
                    try:
                        out.write(pack('i',p[a]))
                    except KeyError:
                        out.write(pack('i',0))
                for a in 'vturn x y d dx dy vx vy potential'.split():
                    try:
                        out.write(pack('f',p[a]))
                    except KeyError:
                        out.write(pack('f',0))

if 1:
  for d in range(50):
    print d
    conv_lev('lev%d'%d)

def conv_model(name='bucket'):
    """Load a model in json format and convert it to a flat structure.

    This involves offseting the indices for subsequent parts of the model.
    The flat structure as three arrays and each array has 3 elements in it."""
    P=[]
    T=[]
    F=[]
    with open(name+'.json','rb') as fd:
        A=json.load(fd)
        for a in A:  # pts is list of vec3, tex list of vec2, faces list of integers
            na,data = a
            [pts,tex,faces] = data[0:3]
            off=len(P)
            P=P+pts
            T=T+tex
            for f in faces:
                F.append([x+off for x in f])
        with open(name+'.pi','wb') as out:
            out.write(pack('ii',len(P),len(F)))
            for p in P:
                out.write(pack('fff',*p))
            for tu,tv in T:
                out.write(pack('fff',tu,tv,1.0))
            for f in F:
                out.write(pack('hhh',*f[0:3]))

for m in 'mainpeng tree babypeng bucket iceblock laser'.split():
    print m
    conv_model(m)
