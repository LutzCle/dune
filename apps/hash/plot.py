#!/bin/python

import sys
import numpy as np
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

insert_file = sys.argv[1]
lookup_file = sys.argv[2]

lines = list()
for i in range(3, len(sys.argv)):
    with open(str(sys.argv[i]), 'r') as fhandle:
        lines.extend( [ l.strip() for l in fhandle ] )
    fhandle.close()

insert_df = pd.DataFrame.from_items([
        ("Native Insert", [int(l.split()[2]) for l in lines if "Native insert" in l]),
        ("Dune Insert", [int(l.split()[2]) for l in lines if "Dune insert" in l])
        ])

lookup_df = pd.DataFrame.from_items([
        ("Native Lookup", [int(l.split()[2]) for l in lines if "Native lookup" in l]),
        ("Dune Lookup", [int(l.split()[2]) for l in lines if "Dune lookup" in l])
        ])

print(insert_df.describe())
print(lookup_df.describe())

sns.stripplot(data=insert_df.sample(n=10000), jitter=True)
sns.plt.savefig(insert_file)
sns.plt.show()
sns.plt.clf()


sns.stripplot(data=lookup_df.sample(n=10000), jitter=True)
sns.plt.savefig(lookup_file)
sns.plt.show()
