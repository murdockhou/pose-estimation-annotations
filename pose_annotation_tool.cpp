/* Copyright 2018
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ===================================================================================
# code reference : https://github.com/motlabs/dont-be-turtle-pose-annotation-tool
*/


#include <stdio.h>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <string.h>
#include <string>


#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

using namespace cv;
using namespace std;

#define JOINTS 14

/* keyboard number */
int ESC     = 27;
int ENTER   = 13;
int SPACE   = 32;
int TAB     = 9;


/*
joint_points[][] =
{
    {x,y,occluded_flag},...
}
*/

int joint_points[JOINTS][3] = {-1};
int current_joint = -1;
int r[2], g[2], b[2];

char joints[JOINTS][20] =
{
        "right shoulder",
        "right elbow",
        "right hand",
        "left shoulder",
        "left elbow",
        "left hand",
        "right hip",
        "right knee",
        "right ankle",
        "left hip",
        "left knee",
        "left ankle",
        "head",
        "neck"
};


void CallBackFunc(int event, int x, int y, int flags, void* param)
{
	if (event == CV_EVENT_LBUTTONUP)
	{
		joint_points[current_joint][0] = x;
		joint_points[current_joint][1] = y;
		joint_points[current_joint][2] = 0;

		int occlusion = 0;
		Mat &img = *((Mat*)(param));
		circle(img, Point(joint_points[current_joint][0], joint_points[current_joint][1]), 10, Scalar(b[occlusion], g[occlusion], r[occlusion]), 3);
		imshow("image", img);
	}
	else if (event == CV_EVENT_RBUTTONUP)
	{
		joint_points[current_joint][0] = x;
		joint_points[current_joint][1] = y;
		joint_points[current_joint][2] = 1;

		int occlusion = 1;
		Mat &img = *((Mat*)(param));
		circle(img, Point(joint_points[current_joint][0], joint_points[current_joint][1]), 10, Scalar(b[occlusion], g[occlusion], r[occlusion]), 3);
		imshow("image", img);
	}

}

void init()
{
	r[0] = 0;
	g[0] = 255;
	b[0] = 255;

	r[1] = 255;
	g[1] = 0;
	b[1] = 255;
}

int main(int argc, char **argv)
{
	init();
	if(argc < 3)
	{
	    printf("1:\tpath to list of images\n");
	    printf("2:\tfile to save annotations (.json)\n");
	    return -1;
	}


	Mat img;

	namedWindow("image",WINDOW_NORMAL);
	namedWindow("Progress",WINDOW_NORMAL);
	setMouseCallback("image", CallBackFunc, &img);

	FILE *fp_list = fopen(argv[1], "r");
	printf("image list path = %s\n", argv[1]);
	int key_pressed = 0;


    FILE *fp_annotation = fopen(argv[2], "w");
    fprintf(fp_annotation, "[");
    bool is_first = true;
    string image_id = "123";
    int count = 1;
	while(!feof(fp_list))
	{
	    char filename[20];
		fscanf(fp_list, "%s", filename);
		printf("filename == %s\n",filename);

		img = imread(filename);
		Mat progress = Mat::zeros(400, 150, CV_8UC3);

		current_joint = -1;
		for(int j = 0; j < JOINTS; j++)
		{
			joint_points[JOINTS][0] = -1;
			joint_points[JOINTS][1] = -1;
			joint_points[JOINTS][2] = -1;
		}

		imshow("image", img);
		imshow("Progress", progress);

        key_pressed =waitKey(0);
		if(key_pressed == SPACE)
		{
			fprintf(stderr, "\nSkipping : %s", filename);
			printf("\n[SPACE] Skipping : %s", filename);
			continue;
		}
		else if(key_pressed == ESC)
		{
			fprintf(stderr, "\nEXITING");
			printf("\n[ESC] EXITING");
            fprintf(fp_annotation, "}");
			break;
		}
        else
		{
			fprintf(stderr, "\nProcessing : %s", filename);
			printf("\n[Enter] Processing : %s", filename);

			if (is_first) is_first = false;
			else fprintf(fp_annotation, "},");

			for(int j=0; j<JOINTS; )
			{
				current_joint = j;
				putText(progress, joints[j], Point(10, 20+j*20), 1, 1, Scalar(255, 255, 255));

				imshow("image", img);
				imshow("Progress", progress);
				key_pressed = waitKey(0);

				int occlusion = joint_points[current_joint][2];
				cout << occlusion << endl;

				if(key_pressed == TAB)
				{
				    fprintf(stderr, "\nSkip this part, set its value to -1 ");
			        printf("\nSkip this part, set its value to -1 ");

					joint_points[current_joint][0] = -1;
					joint_points[current_joint][1] = -1;
					joint_points[current_joint][2] = -1;

					putText(progress, "***********", Point(10, 20+j*20), 1, 1, Scalar(255, 255, 255));
					j++;
					imshow("Progress", progress);
				}
				else
				{
					circle(img, Point(joint_points[current_joint][0], joint_points[current_joint][1]), 10, Scalar(b[occlusion], g[occlusion], r[occlusion]), -1);
					j++;
					// 1 可见， 2 不可见 3 不在图内或不可推测
					if (occlusion == 0) joint_points[current_joint][2] = 1;
					else joint_points[current_joint][2] = 2;
				}
			}
            /* joints saving */
            /* json format writting */
            fprintf(fp_annotation, "{\"image_id\": \"%s\",",image_id.c_str());
            image_id += std::to_string(count++);
            fprintf(fp_annotation, "\"keypoint_annotations\":{\"human1\":[");
			for(int j=0; j<JOINTS; j++) {
                if (j == JOINTS - 1)
                    fprintf(fp_annotation, " %d, %d, %d", joint_points[j][0], joint_points[j][1], joint_points[j][2]);
                else
                    fprintf(fp_annotation, "%d, %d, %d,", joint_points[j][0], joint_points[j][1], joint_points[j][2]);
            }
            fprintf(fp_annotation, "]},\"human_annotations\": {\"human1\": [0, 0, 0, 0]}");
		}
	}
//"human_annotations": {"human2": [0, 27, 266, 851], "human1": [166, 92, 412, 807]}
	fprintf(fp_annotation, "]");
    fclose(fp_annotation);
	fclose(fp_list);
	return 0;
}
