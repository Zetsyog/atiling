# ATiling

Perform source to source algebraic tiling optimization

## Requirements

- Pluto (link)
- Trahrhe (link)

## Build

Configure

```
./get_submodules.sh
./autogen.sh
./configure
```

Now run 
```
make
```
to start compiling and 
```
sudo make install
```
to install as a library (WIP)

## Usage

```
atiling [options] input_file
```
*Character `-` is a special value for `input_file` to get input from stdin* 

Options available:
- *-o, --output* Set output file. Use *stdout* to set output to stdout
- *--transform* Enable loop transformations for better performance
- *-v, --version* Display release information
- *-h, --help* Display program help

## Resources

- Polybench
- Pluto
