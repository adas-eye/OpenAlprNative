#include "NativeAlpr.h"
#include <alpr.h>
#include <string.h>

#include <string.h>
#include <stdio.h>
#include <jni.h>

// open alpr includes
#include "support/filesystem.h"
#include "support/timing.h"


#include "cjson.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>



JNIEXPORT jstring JNICALL Java_com_example_akshika_opencvtest_NativeAlpr_stringFromJNI
  (JNIEnv *env, jobject thiz)
{
	std::string message = "Open Alpr Version:" + alpr::Alpr::getVersion();
	return env->NewStringUTF((message.c_str()));
}


JNIEXPORT jstring JNICALL Java_com_example_akshika_opencvtest_NativeAlpr_recognize(JNIEnv *env,
                                                                     jobject object, jstring jimgFilePath, jint jtopN)
{
    jstring defaultCountry = env->NewStringUTF("us");
    jstring defaultRegion = env->NewStringUTF("");
    jstring defaultConfigFilePath = env->NewStringUTF(CONFIG_FILE);
    return _recognize(env, object, defaultCountry, defaultRegion, jimgFilePath, defaultConfigFilePath, jtopN);
}

JNIEXPORT jstring JNICALL Java_com_example_akshika_opencvtest_NativeAlpr_recognizeWithCountryNRegion(
        JNIEnv *env, jobject object, jstring jcountry,
        jstring jregion, jstring jimgFilePath, jint jtopN)
{
    jstring defaultConfigFilePath = env->NewStringUTF(CONFIG_FILE);
    return _recognize(env, object, jcountry, jregion, jimgFilePath, defaultConfigFilePath, jtopN);
}

JNIEXPORT jstring JNICALL Java_com_example_akshika_opencvtest_NativeAlpr_recognizeWithCountryRegionNConfig
        (JNIEnv *env, jobject object, jstring jcountry, jstring jregion,
         jstring jimgFilePath, jstring jconfigFilePath, jint jtopN)
{
    return _recognize(env, object, jcountry, jregion, jimgFilePath, jconfigFilePath, jtopN);
}

jstring _recognize(JNIEnv *env, jobject object,
                   jstring jcountry, jstring jregion, jstring jimgFilePath,
                   jstring jconfigFilePath, jint jtopN)
{

    const char* countryChars = env->GetStringUTFChars(jcountry, NULL);

    std::string country(countryChars);

    env->ReleaseStringUTFChars(jcountry, countryChars);

    if(country.empty())
    {
        country = "us";
    }

    const char* configFilePathChars = env->GetStringUTFChars(jconfigFilePath, NULL);

    std::string configFilePath(configFilePathChars);

    env->ReleaseStringUTFChars(jconfigFilePath, configFilePathChars);

    if(configFilePath.empty())
    {
        configFilePath = "/etc/openalpr/openalpr.conf";
    }

    const char* imgFilePath = env->GetStringUTFChars(jimgFilePath, NULL);

    int topN = jtopN;

    std::string response = "";

    cv::Mat frame;
    alpr::Alpr alpr(country, configFilePath);

    const char* regionChars = env->GetStringUTFChars(jregion, NULL);

    std::string region(regionChars);

    env->ReleaseStringUTFChars(jregion, regionChars);

    if(region.empty())
    {
        alpr.setDetectRegion(true);
        alpr.setDefaultRegion(region);
    }


    alpr.setTopN(topN);

    if (alpr.isLoaded() == false) {
        env->ReleaseStringUTFChars(jimgFilePath, imgFilePath);
        response = errorJsonString("Error initializing Open Alpr");
        return env->NewStringUTF(response.c_str());
    }

    if(alpr::fileExists(imgFilePath))
    {
        frame = cv::imread(imgFilePath);
        response = detectandshow(&alpr, frame, "");
    }
    else
    {
        response = errorJsonString("Image file not found");
    }
    env->ReleaseStringUTFChars(jimgFilePath, imgFilePath);
    return env->NewStringUTF(response.c_str());
}


std::string detectandshow(alpr::Alpr* alpr, cv::Mat frame, std::string region)
{
    std::string resultJson = "";

    std::vector<char> array;

    timespec startTime;
    alpr::getTimeMonotonic(&startTime);

    alpr::AlprResults results = alpr->recognize(frame);

    timespec endTime;
    alpr::getTimeMonotonic(&endTime);
    double totalProcessingTime = alpr::diffclock(startTime, endTime);

    if (results.plates.size() > 0)
    {
        resultJson = alpr->toJson(results);
    }

    return resultJson;
}

std::string errorJsonString(std::string msg)
{
    cJSON *root;
    root = cJSON_CreateObject();
    cJSON_AddTrueToObject(root, "error");
    cJSON_AddStringToObject(root, "msg", msg.c_str());

    char *out;
    out = cJSON_PrintUnformatted(root);

    cJSON_Delete(root);

    std::string response(out);

    free(out);
    return response;
}
