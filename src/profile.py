# MIT License
# Copyright (c) 2018 Alasdair Macindoe

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
import statistics as s
from collections import Counter

branches = []
takes = []
data = None
occurrences = {}
unique = 0

#Load Data
with open('../data/fft-O0-sample.txt', 'r') as f:
    print("Opened")
    data = f.readlines()

#Clean data
for d in data:
        d.replace("\n", "")
        before, sep, after = d.partition(" ")
        branches.append(int(before))
        takes.append(True if after.replace("\n", "") == '1' else False)
data = None #gc

#Perform basic statistics
print("Mean branch number: {}".format(s.mean(branches)))
print("Standard Deviation of branches: {}".format(s.stdev(branches)))
print("Variance in branches: {}".format(s.variance(branches)))
print("stdev of 10,000 branches: {}".format(s.stdev(branches[:10000])))
print("Variance of 10,000 branches: {}".format(s.variance(branches[:10000])))
print("stdev of last 10,000: {}".format(s.stdev(branches[10001:])))
print("Variance of last 10,000: {}".format(s.variance(branches[10001:])))
print("Number of values: {}".format(Counter(takes).most_common(2)))
print("Number of values for first 10,000: {}".format(Counter(takes[:10000]).most_common(2)))

for b in branches:
    if b in occurrences:
        occurrences[b] += 1
    else:
        occurrences[b] = 1
        unique += 1

print("There are: {} branches that occur more than 100 times".format(len([a for a, b in occurrences.items() if b >= 100])))
common = Counter(occurrences).most_common(31)
print("Top 31 Branches: {}".format(common))
print("Account for: {}/20000".format(sum([c for _,c in common])))
print("There are: {} unique branches".format(unique))

top = Counter(occurrences).most_common(100)
print("Top 100 account for: {}/20000".format(sum([c for _,c in top])))

top = Counter(occurrences).most_common(256)
print("Top 256 account for: {}/20000".format(sum([c for _,c in top])))

top = Counter(occurrences).most_common(512)
print("Top 512 account for: {}/20000".format(sum([c for _,c in top])))

top = Counter(occurrences).most_common(1024)
print("Top 1024 account for: {}/20000".format(sum([c for _,c in top])))

#Longest streak of takes
longest = 0
streaks = []
running = 0
for t in takes:
    if t:
        running += 1
    else:
        if running > longest:
            longest = running
        if running > 1:
            streaks.append(running)
        running = 0

print("The longest streak is: {}".format(longest))
print("We had {} longer than 2".format(len(streaks)))
print("They accounted for: {} branches taken".format(sum(streaks)))

#Longest streak of not takes
longest = 0
streaks = []
running = 0
for t in takes:
    if not t:
        running += 1
    else:
        if running > longest:
            longest = running
        if running > 1:
            streaks.append(running)
        running = 0

print("The longest streak of not taken is: {}".format(longest))
print("We had {} longer than 2".format(len(streaks)))
print("They accounted for: {} branches taken".format(sum(streaks)))
