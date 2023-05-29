import subprocess
from pathlib import Path
from contextlib import contextmanager
import dbus
from pprint import pprint
from xml.etree import ElementTree

def run(*args):
    result = subprocess.run(
        args, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
    )
    # if result.stdout:
    #     print(result.stdout)
    result.check_returncode()
    return result.stdout

def mkfs(output_file):
    # Start off with file of the correct size with all null bytes.
    with output_file.open("wb") as f:
        f.write(bytes(16 * 1024 * 1024))

    # Format as FAT
    run(
        "mkfs.fat",
        "-v",
        "-F",
        "16",
        "--mbr=yes",
        "-n",
        "EXAMPLE",
        "--invariant",
        output_file,
    )

class Bus:
    def __init__(self):
        self.bus = dbus.SystemBus()

    def get_interface(self, path, interface):
        if '.' not in interface:
            interface = 'org.freedesktop.UDisks2.' + interface
        return dbus.Interface(self.bus.get_object('org.freedesktop.UDisks2', path),
                              dbus_interface=interface)

    def list_interfaces(self, path):
        tree = ElementTree.fromstring(
            self.get_interface(path, 'org.freedesktop.DBus.Introspectable').Introspect())
        interfaces = []
        for node in tree.findall('.interface'):
            interfaces.append(node.attrib['name'])
        return interfaces

    def get_property(self, path, interface, property_name, **kwargs):
        return self.get_interface(path, 'org.freedesktop.DBus.Properties').Get(interface, property_name, **kwargs)


def main():
    from argparse import ArgumentParser
    from xml.etree import ElementTree

    parser = ArgumentParser()
    parser.add_argument("output_file", type=Path)
    output_file = parser.parse_args().output_file

    bus = Bus()

    manager = bus.get_interface('/org/freedesktop/UDisks2/Manager',
                                'Manager')
    # Clean up existing loop devices backed by our output file.
    for path in manager.GetBlockDevices({}):
        loop_interface_name = 'org.freedesktop.UDisks2.Loop'
        if loop_interface_name not in bus.list_interfaces(path):
            continue
        backing_file = bus.get_property(path, loop_interface_name, 'BackingFile', byte_arrays=True)
        # File path has a null terminator
        backing_file = backing_file.rstrip(b'\0').decode('utf-8')
        if backing_file != str(output_file):
            continue
        print(f"Deleting loop device {path} because it's backed by {output_file}")
        loop = bus.get_interface(path, loop_interface_name)
        loop.Delete({})

    # Create filesystem
    mkfs(output_file)

    # Bind loop device
    with open(output_file, 'rb+') as f:
        block_path = manager.LoopSetup(f.fileno(), {})
    partition_path = bus.get_property(block_path, 'org.freedesktop.UDisks2.PartitionTable', 'Partitions')[0]
    fs = bus.get_interface(partition_path, 'org.freedesktop.UDisks2.Filesystem')

    mount_path = fs.Mount({})

    (Path(mount_path) / "file.txt").write_text('hello world')


    fs.Unmount({})




if __name__ == "__main__":
    main()
