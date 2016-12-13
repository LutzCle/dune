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
        ("Native Insert Small", [int(l.split()[3]) for l in lines if "Native insert small" in l]),
        ("Dune Insert Small", [int(l.split()[3]) for l in lines if "Dune insert small" in l]),
        ("Native Insert Huge", [int(l.split()[3]) for l in lines if "Native insert huge" in l]),
        ("Dune Insert Huge", [int(l.split()[3]) for l in lines if "Dune insert huge" in l])
        ])

print(insert_df.describe(percentiles=[.25, .5, .75, .99]))
insert_pct = (insert_df.T / insert_df["Native Insert Small"]).T
print(insert_pct.describe(percentiles=[.25, .5, .75, .99]))

sns.stripplot(data=insert_df.sample(n=10000), jitter=True)
sns.plt.ylim(0, 10000)
sns.plt.title("Hash Table Insert")
sns.plt.ylabel("Cycles")
sns.plt.savefig(insert_file)
sns.plt.show()

sns.plt.clf()

lookup_df = pd.DataFrame.from_items([
        ("Native Lookup Small", [int(l.split()[3]) for l in lines if "Native lookup small" in l]),
        ("Dune Lookup Small", [int(l.split()[3]) for l in lines if "Dune lookup small" in l]),
        ("Native Lookup Huge", [int(l.split()[3]) for l in lines if "Native lookup huge" in l]),
        ("Dune Lookup Huge", [int(l.split()[3]) for l in lines if "Dune lookup huge" in l])
        ])

print(lookup_df.describe(percentiles=[.25, .5, .75, .99]))
lookup_pct = (lookup_df.T / lookup_df["Native Lookup Small"]).T
print(lookup_pct.describe(percentiles=[.25, .5, .75, .99]))

sns.stripplot(data=lookup_df.sample(n=10000), jitter=True)
sns.plt.ylim(0, 10000)
sns.plt.title("Hash Table Lookup")
sns.plt.ylabel("Cycles")
sns.plt.savefig(lookup_file)
sns.plt.show()

