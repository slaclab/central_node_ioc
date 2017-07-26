from os import path
from pydm import Display
from power_class_list_item import PowerClassListItem
from mitigation_list_item import MitigationListItem
import argparse

class MitigationPanel(Display):
  def __init__(self, mitigation_list=[], power_class_list=[], parent=None, args=[]):
    super(MitigationPanel, self).__init__(parent=parent)
    parsed_args = self.parse_args(args)

    if parsed_args.power_class_list:
      power_class_list = self.list_from_file(parsed_args.power_class_list)
    for power_class in power_class_list:
      if not power_class[0].startswith('#'):
        list_item = PowerClassListItem(power_class_pv=power_class[0], parent=self)
        self.powerClassList.layout().addWidget(list_item)
    self.powerClassList.layout().addStretch(0)

    if parsed_args.mitigation_list:
      mitigation_list = self.list_from_file(parsed_args.mitigation_list)
    for mitigation in mitigation_list:
      if not mitigation[0].startswith('#'):
        list_item = MitigationListItem(mitigation_label=mitigation[0], mitigation_pv=mitigation[1], parent=self)
        self.mitigationDeviceList.layout().addWidget(list_item)
    self.mitigationDeviceList.layout().addStretch(0)

#    self.scrollArea.setMinimumWidth(self.faultList.minimumSizeHint().width())
  
  def parse_args(self, args):
    parser = argparse.ArgumentParser()
    parser.add_argument('--power-class-list', dest='power_class_list', help='File containing a list of power class PVs.')
    parser.add_argument('--mitigation-list', dest='mitigation_list', help='File containing a list of migation device PVs.')
    parsed_args, _unknown_args = parser.parse_known_args(args)
    print(parsed_args)
    return parsed_args
  
  def list_from_file(self, filename):
    if not path.isabs(filename):
      filename = path.join(path.dirname(__file__), filename)
    lines = []
    with open(filename) as f:
      lines = f.readlines()
    return [l.strip().split('|') for l in lines]  
    
  def ui_filename(self):
    return 'mitigation-panel.ui'
    
  def ui_filepath(self):
    return path.join(path.dirname(path.realpath(__file__)), self.ui_filename())

intelclass = MitigationPanel
