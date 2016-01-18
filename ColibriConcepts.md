# Concepts -- A Tool for Formal Concept Analysis #

Concepts is a command line tool for concept analysis. Concept Analysis helps to analyze the relation between objects with assigned attributes by computing a so-called concept lattice. The concept lattice identifies sets of objects and attributes that are synonymous for each other. For a more detailed explanation and an example, see below. Concepts main features are:

  * A mature, portable implementation in C (K&R and ANSI)
  * Speed - it implements Ganter's Next Closure Algorithm
  * Flexible output format, controlled by a format string
  * Emits lattices in DOT format for layout with [Graphviz](http://www.graphviz.org/)

If you are curious, you can also take a look at the Unix-style ManualPage.

# Download and Installation #

Concepts is implemented in C and should compile on most Unix platforms. It comes with a ./configure script and a manual page. You can download the source code from this Subversion repository:

```
svn checkout http://colibri-concepts.googlecode.com/svn/trunk/ colibri-concepts
```

To compile it, change to directory `trunk` or `stable`  and execute the following commands:
```
./configure
make
make install
```

This will install the binary `concepts` in the `/usr/local/` hierarchy, together with a ManualPage.

# Concept Analysis #

Concept analysis computes a so-called concept lattice from a binary relation, like the following:

```
chmod:   change file mode permission ;
chown:    change file group owner;
fstat:	      get file status ;
fork:	      create new process ;
chdir:     change directory ;
mkdir:    create directory new ;
open:     create file open read write;
read:	     file input read ;
rmdir:   directory file remove ;
write:     file output write ;
creat:     create file new ;
access:   access check file ;
```

The relation above assigns attributes (or features) to objects. Objects are to the left of the colon, and attriutes to the right. This particular relation assigns keywords to Unix system calls. Concepts computes the following set of concepts for this relation, where a concept is a pair of an object set and an attribute set.

```
{0 {access creat write rmdir read open mkdir chdir fork fstat chown chmod} {}}
{1 {access creat write rmdir read open fstat chown chmod} {file}}
{2 {access} {file check access}}
{3 {creat open mkdir fork} {create}}
{4 {creat open} {file create}}
{5 {creat mkdir fork} {new create}}
{6 {creat} {file new create}}
{7 {write open} {file write}}
{8 {write} {file write output}}
{9 {rmdir mkdir chdir} {directory}}
{10 {rmdir} {file remove directory}}
{11 {read open} {file read}}
{12 {read} {file read input}}
{13 {open} {file create write read open}}
{14 {mkdir} {new create directory}}
{15 {chdir chown chmod} {change}}
{16 {chdir} {directory change}}
{17 {fork} {new create process}}
{18 {fstat} {file status get}}
{19 {chown chmod} {file change}}
{20 {chown} {file change owner group}}
{21 {chmod} {file change permission mode}}
{22 {} {file check access new create write output remove directory
        read input open change process status get owner group permission mode}}
```




Any set of objects shares a (possibly empty)
set of common attributes: the set {chdir,mkdir} shares {directory} for
example. The same is true for attribute sets which share common
objects. So every set of objects determines a set of common attributes
and every set of attributes determines a set of common objects,
forming a pair of object and attribute set. A pair (O,A) of such two
sets is called a concept, if the following holds: the set of
attributes common to the objects in O is A and the set of objects
commonly shared by the attributes in A is O. Every binary relation
induces such concepts and the program calculates them.

All concepts from a binary relation are (partially) ordered and even
more, form a lattice. Concepts are ordered by (O1,A1) <= (O2,A2) if and
only if A1 <= A2 holds. Concepts computes for every concept its super-
and subconcepts and permits to output the lattice structure. The
complete set of concepts from the example above is shown below. Each
concept contains a unique number, the set of objects and the set of
attributes.

# Theory #

The definite textbook for concept analysis is ''Formal Concept Analysis : Mathematical Foundations''
by  Bernhard Ganter, Rudolf Wille, C. Franzke, Springer, 1998. Concepts implements Ganter's algorithm as it is presented in this book. My paper [Fast Concept Analysis](http://www.st.cs.uni-sb.de/publications/details/lindig-fca-2000/) proposes a more efficient algorithm which, however, is not yet implemented in C.


# Copyright #

Concepts is distributed under the GNU General Public License (source:concepts/trunk/COPYING).

# Author #

Christian Lindig <lindig at gmail dot com> http://www.cs.uni-sb.de/~lindig/


