from os import path
from pydm import Display
from PyQt4.QtGui import QDialog

import argparse

class BypassPanel(Display):
  def __init__(self, bypass_pv, parent=None):
    super(BypassPanel, self).__init__(parent=parent)

    if bypass_pv:
      self.currentStateLabel.channel = "ca://{name}".format(name=bypass_pv)
      self.bypassValueLabel.channel = "ca://{name}_BYPV".format(name=bypass_pv)
      self.bypassStateLabel.channel = "ca://{name}_BYPS".format(name=bypass_pv)

#    self.scrollArea.setMinimumWidth(self.faultList.minimumSizeHint().width())
  
  def ui_filename(self):
    return 'bypass-panel.ui'

  def ui_filepath(self):
    return path.join(path.dirname(path.realpath(__file__)), self.ui_filename())

intelclass = BypassPanel
