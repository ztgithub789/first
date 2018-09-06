/*
 * RoadPassion CONFIDENTIAL
 *
 * Copyright 2013 RoadPassion Electronics Co., Ltd.
 *
 * DO NOT COPY AND/OR REDISTRIBUTE WITHOUT PERMISSION.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#ifndef BUILTINCOMMANDS_H_
#define BUILTINCOMMANDS_H_

#include <stdio.h>
#include <utils/Shell/Shell.h>

class HelpCommand : public Command {
public:
	const char *GetName(void) { return "help"; }
	const char *GetHelpStr(void) { return "Print this help text"; }

	int Execute(const int argc, const char *argv[])
	{
		printf("== Help Contents ==\n");
		Command **list = Shell::getInstance()->GetCommandList();
		int i;

		for (i=0; i<Shell::getInstance()->GetCommandNr(); i++) {
			printf("%s\t--\t%s\n", list[i]->GetName(), list[i]->GetHelpStr());
		}

		return 0;
	}
};

/**
 * @brief  Version Definition
 * @note   Major Version: VERSION / 100
 *         Sub Version:   VERSION % 100
 * @param  None
 * @retval None
 */
class versionCommand : public Command {
public:
	const char *GetName(void) { return "version"; }
	const char *GetHelpStr(void) { return "Show version"; }

	int Execute(const int argc, const char *argv[])
	{
		printf("ZShell - Ver. %d.%d \n\n", versionNr/100, versionNr%100);
		return 0;
	}

private:
	static const int versionNr = 100;

};


#endif /* BUILTINCOMMANDS_H_ */
