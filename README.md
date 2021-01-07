# mapmirror
This allows a configurable and repeatable find-and-replace on quake maps.

Usage:
`$ ./mapmirror [options] <mapname.map>`

Possible options are:
* -h | --help: display help and exit
* -d: print extra debugging info
* -c <file>: specify a config file (default is mapmirror.conf)
* -o <file>: specify output map filename (default is out.map)

# Config
The program works in 2 ways:
- replacing all instances of a specified texture with another
- replacing entity fields for entities that match certain criteria

These are both configured in the config file (mapmirror.conf by default)
Firstly, lines starting with # are comments and are ignored.
The [textures] section defines the texture replacement rules. Each rule is specified on 2 lines - what the old and the new texture. Multiple replacement rules can be defined consecutively.

For example:
```
[textures]
BANNER3
BANNER4
BANNER5
BANNER6
+0flattele
FLOOR23
```
Will replace BANNER3 with BANNER4, +0flattele with FLOOR23 etc

The [fields] section definition uses 4 lines:
- classname: required - the entity classname
- fieldname: required - the field name within the entity
- oldvalue: optional - if not blank, will have to match exactly to be replaced
- newvalue: required - the new value to fill in

Multiple rules can be defined, even for the same classname. Each match is global.

For example:
```
[fields]
func_door
team_no
2
1
func_door
dmg

888
```
This will replace any func_door's dmg field with the value "888" (but not add it), and will change the team_no to "1" only on those doors where team_no is currently "2".