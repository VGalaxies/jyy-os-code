#!/bin/bash

# Create directories
mkdir -p A B C D E F

# Create automaton
ln -s ../B 'A/<'
ln -s ../C 'B/>'
ln -s ../D 'C/<'
ln -s ../E 'A/>'
ln -s ../F 'E/<'
ln -s ../D 'F/>'
ln -s ../A 'D/_'
