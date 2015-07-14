## lsaddr(1)

`lsaddr` lists active IP addresses of the specified `interface`s. By default, it lists all IPv4
and IPv6 addresses, with the loopback interfaces and IPv6 link-local omitted.

The `interface` names are specified as in `ifconfig(8)`: usually a driver name
followed by a number, eg, `eth0`. Alias interfaces are not supported.

## Usage
`lsaddr [-46] [--include-loopback] [--include-ipv6-link-local] [interface ...]`

`lsaddr --list-interfaces`

## Options
* `-4`, `--ipv4` :: list IPv4 addresses
* `-6`, `--ipv6` :: list IPv6 addresses
* `--include-loopback` :: include addresses for the loopback interface
* `--include-link-local` :: include IPv6 link local addresses
* `--list-interfaces` :: list interfaces and exit
