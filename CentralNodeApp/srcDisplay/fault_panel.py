from os import path
from pydm import Display
from fault_list_item import FaultListItem
import argparse

class FaultPanel(Display):
  def __init__(self, fault_list=[], parent=None, args=[]):
    super(FaultPanel, self).__init__(parent=parent)
    parsed_args = self.parse_args(args)
    if parsed_args.fault_list:
      fault_list = self.fault_list_from_file(parsed_args.fault_list)
    for fault in fault_list:
      if not fault[0].startswith('#'):
        list_item = FaultListItem(fault_description=fault[1], fault_pv=fault[0], parent=self)
        self.faultList.layout().addWidget(list_item)
    self.faultList.layout().addStretch(0)
    self.scrollArea.setMinimumWidth(self.faultList.minimumSizeHint().width())
  
  def parse_args(self, args):
    parser = argparse.ArgumentParser()
    parser.add_argument('--list', dest='fault_list', help='File containing a list of fault PV names to use.')
    parsed_args, _unknown_args = parser.parse_known_args(args)
    print(parsed_args)
    return parsed_args
  
  def fault_list_from_file(self, filename):
    if not path.isabs(filename):
      filename = path.join(path.dirname(__file__), filename)
    lines = []
    with open(filename) as f:
      lines = f.readlines()
    return [l.strip().split('|') for l in lines]  
    
  def ui_filename(self):
    return 'fault-panel.ui'
    
  def ui_filepath(self):
    return path.join(path.dirname(path.realpath(__file__)), self.ui_filename())

intelclass = FaultPanel
