A brief description of the directories and files in the /firmware/projects 
directory.



AudioMonitor/
  This directory contains a firmware example which creates a simple audio 
  monitor for the DICE MINI Audio Monitor target board. 

  It has 2 channels of Headphone out and a stereo SPDIF input.

  This project uses the DiceDriver model.



template/
  This directory contains the project templates which are used by the 
  new_xxx_yyy_proj.sh scripts to generate new starting-point firmware 
  projects.


install.sh
  This script is run by the developer when the firmware sources are first 
  installed from the repository. It makes example projects out of many of 
  the templates, and builds them. It also builds the RedBoot and memory 
  test binaries.


copyProject.sh
  ***** This script should be used to create new projects from template
  projects.

  For usage:

  $ ./copyProj.sh

  It is also useful to rename or copy an existing project, for example 
  when a developer wishes to create a variant of an existing product.

  This script is called by all of the new_xxx_yyy_proj.sh scripts, which
  are included for backward compatibility.


- o -


createBinaries.sh
  Used to rebuild all of the firmware projects created by the install.sh 
  script.

  This is usually not used by the developer.

new_avc_tcd22x0_proj.sh
  Included for backward compatibility.

  Creates a firmware project from the avc_tcd22x0 template project for use 
  with Dice-Jr and Dice-Mini, using the EVM002.

  This project is based on the AV/C driver model.


new_cms_tcd22x0_proj.sh
  Included for backward compatibility.

  Creates a firmware project from the cms_tcd22x0 template project for use 
  with Dice-Jr and Dice-Mini, using the EVM002.

  The firmware implements the CMS protocol for configuration and monitoring 
  by Hosts (async-only with host).

  This project is for peer-to-peer streaming and has no host streaming 
  driver model.


new_diceII_EVM002_proj.sh
  Included for backward compatibility.

  Creates a firmware project from the diceII_EVM002 template project for 
  use with DiceII, using the EVM002.

  This project is based on the DiceDriver driver model.


new_diceII_proj.sh
  Included for backward compatibility.

  Creates a firmware project from the diceII template project for use with 
  DICEII, using the DICEII Classic EVM.

  This project is based on the DiceDriver driver model.


new_eap_tcd22x0_proj.sh
  Included for backward compatibility.

  Creates a firmware project from the eap_tcd22x0 template project for use 
  with Dice-Jr and Dice-Mini, using the EVM002.

  The firmware implements the EAP protocol for control and monitoring of the 
  hardware mixer, and accessing the peak detector.

  This project is based on the DiceDriver driver model.


new_no1394_tcd22x0_proj.sh
  Included for backward compatibility.

  Creates a firmware project from the no1394_tcd22x0 template project for 
  use with Dice-Jr and Dice-Mini, using the EVM002.

  This project does not have a host driver model implementation, and does 
  not commuicate on the 1394 bus.

  It is intended to be a starting point for products that do not use a 1394 
  PHY, and only use the DICE audio routing/mixing/PLL capabilities.


new_tcd22x0_proj.sh
  Included for backward compatibility.

  Creates a firmware project from the tcd22x0 template project for use with 
  Dice-Jr and Dice-Mini, using the EVM002.

  This project is based on the DiceDriver driver model.
