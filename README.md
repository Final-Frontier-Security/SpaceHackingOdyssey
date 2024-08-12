# SpaceHackingOdyssey
Thank you for attending our class! The resources in this repository can be used to review class material or create your own environment for exploring cyber security in space vehicles.\
\
## Environment Setup
If you would like to spin up your own environment in AWS, you will need the following:

* An AWS account that is not limited to the free tier (unfortunately, running the flight software is quite CPU intensive)
* The AWS comand line utility installed locally (instructures here - https://docs.aws.amazon.com/cli/latest/userguide/getting-started-install.html)
* AWS credentials configured locally (https://docs.aws.amazon.com/cli/latest/userguide/cli-chap-configure.html)
* git installed (https://github.com/git-guides/install-git)

### Cloning the repo
Open a local terminal and run the following command to download all the files in this repo:\
```git clone git@github.com:Final-Frontier-Security/SpaceHackingOdyssey.git```

### Uploading SSH Key
The EC2 instances that are deployed by the CloudFormation template require you to have an SSH key configured with the name "bhu24". You can create your own key with that name or upload one using the bhus24.pub key material.\
From the same termianl window, you can run the following to create the key:\
```cd SpaceHackingOdyssey```\
```aws ec2 import-key-pair --key-name bhus24 --public-key-material fileb://bhus24.pub --region us-east-1```

### Deploying the Environment
Again, from that same terminal window, run the following AWS commands to start to deploy the environment. You may need to specify a region:\
```aws cloudformation create-stack --stack-name bhus24-stack --template-body file://BlackHat-2024-satellite-environment.yaml --capabilities CAPABILITY_NAMED_IAM```\
```aws cloudformation deploy --template-file file://BlackHat-2024-satellite-environment.yaml --stack-name bhus24-stack --capabilities CAPABILITY_NAMED_IAM```

