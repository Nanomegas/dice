//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2006 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************

 DESCRIPTION: 
	This file is just a page for doxygen and is not part of the
	source tree.

****************************************************************************/

/** @page CliUsage	Command Line Prompt Usage

For a detailed description of the CLI commands, see the <strong><i>DICE Firmware 
CLI Reference</i></strong>.

The DICE Firmware Command Line Interface (CLI) allows developers to manually configure 
functions in the DICE. The table in the DICE Firmware CLI Reference document gives a brief 
description of each CLI command. <br> 
 
<hr>
<h2>
Syntax
</h2>
CLI commands, variables and parameters are case-sensitive. For example, 
@code
> echo On
@endcode
will result in a parameter error, and 
@code
> echo on
@endcode
should be used. <br>
 
Commands that have subfunctions are separated with either a period ‘<strong>.</strong>’ or 
a colon ‘<strong>:</strong>’ <br>
 
For example, the following commands are equivalent: 
@code
> lhl.br
> lhl:br 
@endcode

<h2>
Built-in commands 
</h2>
Note that “<strong>built-in</strong>” does not need to be typed in the command-line when 
using commands in this category.  However, all other categories require that the category 
name be typed as part of the command line, for example
@code
> dal.start 0
@endcode 

<h2>
Optional commands 
</h2>
Not all commands listed here are available in all DICE Applications. The available 
commands depend on which build options are set in the firmware when the application 
is made. <br>
 
The table below indicates which target and additional flags are necessary to enable 
the command if it is not enabled by default. <br>
 
In general each module has its own CLI tool <strong>#define</strong>. For example, 
to enable to CLI tools for the <strong>pb</strong> module, define <strong>_CLI_TOOL_PB</strong> 
in <strong>cliToolDefs. h</strong>  <br>
 
Also see <strong>Make.params</strong>, and <strong>#defines</strong> in the source code, 
to determine which modules are used. For example, if the application is built with 
the <strong>_DICE_DRIVER</strong> driver model, then the “<strong>avc</strong>” and 
related CLI commands such as “<strong>panel</strong>” are not available. <br>
 
The CLI can be removed altogether to save memory requirements in production 
applications by removing the <strong>_CLI</strong> flag in <strong>Make.params</strong>. <br>

<h2>
Command-line editing
</h2>
The CLI saves your last 9 commands in its history buffer. You can recall previous 
commands using the up/down arrow keys, and you can edit the command line as well. 
Note that you must use a serial terminal program that supports sending terminal keys 
to recall commands. HyperTerminal does not support this, although HyperTerminal PE 
and many other programs do. <br>

For a detailed description of the CLI commands, see the <strong><i>DICE Firmware 
CLI Reference</i></strong>.


*/
