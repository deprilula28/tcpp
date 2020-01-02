#include <catch2/catch.hpp>
#include "tcppLibrary.hpp"
#include <iostream>


using namespace tcpp;


TEST_CASE("Preprocessor Tests")
{
	auto errorCallback = []()
	{
		REQUIRE(false);
	};

	SECTION("TestProcess_PassSourceWithoutMacros_ReturnsEquaivalentSource")
	{
		std::string inputSource = "void main/* this is a comment*/(/*void*/)\n{\n\treturn/*   */ 42;\n}";
		StringInputStream input(inputSource);
		Lexer lexer(input);

		Preprocessor preprocessor(lexer, errorCallback);
		REQUIRE(!preprocessor.Process().empty());
	}

	SECTION("TestProcess_PassSourceWithSimpleMacro_ReturnsSourceWithExpandedMacro")
	{
		std::string inputSource = "#define VALUE 42\n void main()\n{\n\treturn VALUE;\n}";
		StringInputStream input(inputSource);
		Lexer lexer(input);

		Preprocessor preprocessor(lexer, errorCallback);
		std::cout << preprocessor.Process() << std::endl;
	}

	SECTION("TestProcess_PassSourceWithCorrectFuncMacro_ReturnsSourceWithExpandedMacro")
	{
		std::string inputSource = "#define ADD(X, Y) X + Y\n void main()\n{\n\treturn ADD(2, 3);\n}";
		StringInputStream input(inputSource);
		Lexer lexer(input);

		Preprocessor preprocessor(lexer, errorCallback);
		std::cout << preprocessor.Process() << std::endl;
	}

	SECTION("TestProcess_PassSourceWithIncludeDirective_ReturnsSourceStringWithIncludeDirective")
	{
		std::string inputSource = "#include <system>\n#include \"non_system_path\"\n void main()\n{\n\treturn ADD(2, 3);\n}";
		StringInputStream input(inputSource);
		Lexer lexer(input);

		const std::tuple<std::string, bool> expectedPaths[]{ { "system", true }, { "non_system_path", false } };
		short currExpectedPathIndex = 0;

		Preprocessor preprocessor(lexer, errorCallback, [&input, &currExpectedPathIndex, &expectedPaths](const std::string& path, bool isSystem)
		{
			auto expectedResultPair = expectedPaths[currExpectedPathIndex++];

			REQUIRE(path == std::get<std::string>(expectedResultPair));
			REQUIRE(isSystem == std::get<bool>(expectedResultPair));
			return &input;
		});
		preprocessor.Process();
	}

	SECTION("TestProcess_PassSourceWithIncludeDirective_ReturnsSourceStringWithIncludeDirective")
	{
		std::string inputSource = "__LINE__\n__LINE__\n__LINE__";
		StringInputStream input(inputSource);
		Lexer lexer(input);

		Preprocessor preprocessor(lexer, errorCallback);
		REQUIRE(preprocessor.Process() == "1\n2\n3");
	}
}