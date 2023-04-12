#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "IRepka.h"
#include "command_responsive.h"


class IRepkaTest : public ::testing::Test {
protected:

    void SetUp() override {
        repka_ = new IRepka();
    }

    // Удаление объекта IRepka после каждого теста
    void TearDown() override {
        delete repka_;
    }

    IRepka* repka_;

    //key
    std::string key1 = "key1";
    std::string key2 = "key2";
    std::string key3 = "key3";
    //value
    std::string value1 = "value1";
    std::string value2 = "value2";
    std::string value3 = "value3";
};




TEST_F(IRepkaTest, AddElementTest) {
    EXPECT_EQ(repka_->get(key1), "Doesn't exist");
    repka_->set(key1,value1); // Добавляем элемент
    EXPECT_EQ(repka_->get(key1), value1);

    EXPECT_EQ(repka_->get(key2), "Doesn't exist");
    repka_->set(key2,value2); // Добавляем элемент
    EXPECT_EQ(repka_->get(key2), value2);
}

TEST_F(IRepkaTest, AddNewValueInAlreadyExistKey) {
    EXPECT_EQ(repka_->get(key1), "Doesn't exist");
    repka_->set(key1,value1); // Добавляем элемент
    EXPECT_EQ(repka_->get(key1), value1);

    repka_->set(key1,value2); // Добавляем новое значение по существующему ключу
    EXPECT_EQ(repka_->get(key1), value2);
}

TEST_F(IRepkaTest, keyDoesntExist)
{
  // Проверка получения значения с несуществующим ключом
  EXPECT_EQ(repka_->get(""), "Doesn't exist");

  // Проверка удаления значения с неверным ключом
  EXPECT_EQ(repka_->del(""), "Doesn't exist");
}

TEST_F(IRepkaTest, deleteElementSuccsess)
{
    EXPECT_EQ(repka_->get(key1), "Doesn't exist");
    repka_->set(key1,value1);
    repka_->set(key2,value2);
    EXPECT_EQ(repka_->get(key1), value1);
    repka_->del(key1);// удаляем
    repka_->del(key2);// удаляем
    EXPECT_EQ(repka_->get(key1), "Doesn't exist");
    EXPECT_EQ(repka_->get(key2), "Doesn't exist");
}

TEST_F(IRepkaTest, validCountTest)
{
    EXPECT_EQ(repka_->count(), "0");
    repka_->set(key1,value1);
    repka_->set(key2,value2);
    EXPECT_EQ(repka_->count(), "2");
    repka_->del(key1);// удаляем
    EXPECT_EQ(repka_->count(), "1");
    repka_->del(key2);// удаляем
    EXPECT_EQ(repka_->count(), "0");
}   

TEST_F(IRepkaTest, saveAndLoadDumpFile)
{
    repka_->set(key1,value1);
    repka_->set(key2,value2);
    EXPECT_EQ(repka_->count(), "2");
    repka_->save_dump("saveload");
    repka_->del(key1);// удаляем
    repka_->del(key2);// удаляем
    EXPECT_EQ(repka_->count(), "0");
    repka_->load_dump("saveload");
    EXPECT_EQ(repka_->count(), "2");
    EXPECT_EQ(repka_->get(key1), value1);
    EXPECT_EQ(repka_->get(key2), value2);
} 


class CommandResponsiveIRepkaPlug : public IRepka {
public:
    std::string set(const std::string& key, const std::string& val) override {return "SetSuccess";}
	std::string get(const std::string& key) override {return "GetSuccess";} 
	std::string del(const std::string& key) override {return "DelSuccess";} 
	std::string count() override {return "CountSuccess";}
	std::string save_dump(const std::string& filename) override {return "SaveSuccess";}
	std::string load_dump(const std::string& filename) override {return "LoadSuccess";}

};

class CommandResponsiveFixture : public ::testing::Test {
protected:
    CommandResponsiveIRepkaPlug repka;
    char buf[256] {};
};


TEST_F(CommandResponsiveFixture, SetValidCommandTest) {

    strcpy(buf, "set key value");
    std::string result = command_response(repka, buf);
    EXPECT_EQ(result, "SetSuccess");
}

TEST_F(CommandResponsiveFixture, SetNotValidCommandTest) {

    strcpy(buf, "set #@!$ value");
    std::string result = command_response(repka, buf);
    EXPECT_NE(result, "SetSuccess");
}

TEST_F(CommandResponsiveFixture, GetValidCommandTest) {

    strcpy(buf, "get name");
    std::string result = command_response(repka, buf);
    EXPECT_EQ(result, "GetSuccess");
}

TEST_F(CommandResponsiveFixture, GetNotValidCommandTest) {

    strcpy(buf, "get каво");
    std::string result = command_response(repka, buf);
    EXPECT_NE(result, "GetSuccess");
}

TEST_F(CommandResponsiveFixture, CountCommandTest) {

    strcpy(buf, "count");
    std::string result = command_response(repka, buf);
    EXPECT_EQ(result, "CountSuccess");
}

TEST_F(CommandResponsiveFixture, SaveLoadCommandTest) {

    strcpy(buf, "save_dump filename");
    std::string result = command_response(repka, buf);
    EXPECT_EQ(result, "SaveSuccess");
    memset(buf,0,sizeof(buf));
    strcpy(buf, "load_dump filename");
    result = command_response(repka, buf);
    EXPECT_EQ(result, "LoadSuccess");
}
