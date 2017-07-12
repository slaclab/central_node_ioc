from os import path
from pydm import Display
from device_input_list_item import DeviceInputListItem
import argparse

class DeviceInputPanel(Display):
  def __init__(self, device_list=[], parent=None, args=[]):
    super(DeviceInputPanel, self).__init__(parent=parent)
    parsed_args = self.parse_args(args)
    if parsed_args.device_list:
      device_list = self.device_list_from_file(parsed_args.device_list)
    for device in device_list:
      if not device[0].startswith('#'):
        list_item = DeviceInputListItem(device_name=device[4], device=device[0], crate=device[1],
                                        card=device[2], channel=device[3], parent=self)
        self.deviceInputList.layout().addWidget(list_item)
    self.deviceInputList.layout().addStretch(0)
    self.scrollArea.setMinimumWidth(self.deviceInputList.minimumSizeHint().width())
  
  def parse_args(self, args):
    parser = argparse.ArgumentParser()
    parser.add_argument('--list', dest='device_list', help='File containing a list of device input PV names to use.')
    parsed_args, _unknown_args = parser.parse_known_args(args)
    print(parsed_args)
    return parsed_args
  
  def device_list_from_file(self, filename):
    if not path.isabs(filename):
      filename = path.join(path.dirname(__file__), filename)
    lines = []
    with open(filename) as f:
      lines = f.readlines()
    return [l.strip().split('|') for l in lines]  
    
  def ui_filename(self):
    return 'device-input-panel.ui'
    
  def ui_filepath(self):
    return path.join(path.dirname(path.realpath(__file__)), self.ui_filename())

intelclass = DeviceInputPanel
