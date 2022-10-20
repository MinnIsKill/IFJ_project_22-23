# Examples of abstract syntax tree
This folder contain example php programs and script(`gen.sh`) that will generate their graphical representation.

## dependency
In order to run `gen.sh` you need : [graphviz](https://graphviz.org/)

## gen.sh
This script will generate graphical representation(`.png`) of abstract syntax tree for each `.php` files in this directory.
If some files could not be parsed by compiler script will generate broken png image.

## remove all generated pictures
To remove all pictures simply `rm ./*.png`
