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


#ifndef TESTCASE_H_
#define TESTCASE_H_

class TestSuite;

/**
 * 用于判断单条测试是否通过。v==true，表示通过，否则表示不通过。
 * 一个TestCase中，所有的ZUNIT_ASSERT()必须全部通过，该TestCase才算通过。
 */
#define ZUNIT_ASSERT(v)	do { \
								if ((v)!=true) { \
									_result = false; \
									printf("  Assertion failed: %d@%s\n", __LINE__, __FILE__); \
								} \
							} while(0)

class TestCase
{
public:
	TestCase();
	virtual ~TestCase();
	virtual void setUp (void) {}
	virtual void tearDown(void) {}
	virtual void run(void) {}
	const char *getName(void) { return _name; }					///< 获取名字
	void setSuite(const TestSuite *pSuite) { _pSuite = pSuite; }	///< 设置suite
	const TestSuite *getSuite(void) { return _pSuite; }			///< 获取suite
	bool getResult(void) { return _result; }
protected:
	bool _result;				///< 本testcase的结果。true为通过，false为失败。
	const char *_name;			///< 名字
	const TestSuite *_pSuite;	///< 包含该case的suite
};
#endif /* TESTCASE_H_ */
