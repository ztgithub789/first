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


#ifndef SHELL_H_
#define SHELL_H_

#define MAX_CMD_STR_LEN	128		// 命令字符串最大长度
#define MAX_CMD_NR			128		// 最大命令数

static const char PROMPT	= '#';
static const char KEY_BACKSPACE	= 0x7f;
static const char KEY_ENTER		= '\r';
static const char KEY_CTRL_C		= 0x3;


#define MAX_CMD_SEGMENT_LEN	        16      // 命令名以及单个参数的最大长度
#define MAX_CMD_ARG_NR					16		// 命令对大参数个数

class Command {
public:
	Command();
	virtual ~Command() {}
	virtual int Execute(const int argc, const char *argv[]) = 0;
	virtual const char *GetName(void) = 0;
	virtual const char *GetHelpStr(void) = 0;
};

class Shell {
public:
	static Shell *getInstance(void)
	{
		static Shell instance;
		return &instance;
	}
	int registerCommand(Command *cmd);
	void ShowBanner(void);
	void run(void);
	int CallCmd(const char *cmdStr);
	Command **GetCommandList(void);
	int GetCommandNr(void);

protected:
	Shell();
	virtual ~Shell();

private:
	char cmdStr[MAX_CMD_STR_LEN];
	int  currentPos;

	int HandleCmdStr(const char *cmdStr);
	int ParseCmdStr(const char *cmdStr);
	Command *commandList[MAX_CMD_NR];
	int commandNr;						// 已注册命令数
};


#endif /* SHELL_H_ */
