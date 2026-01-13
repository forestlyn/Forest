import os
import subprocess
import sys
from pathlib import Path

import Utils

from io import BytesIO
from urllib.request import urlopen
from zipfile import ZipFile

VULKAN_SDK = os.environ.get("VULKAN_SDK")
VULKAN_SDK_INSTALLER_URL = "https://sdk.lunarg.com/sdk/download/1.4.335.0/windows/vulkansdk-windows-X64-1.4.335.0.exe"
HAZEL_VULKAN_VERSION = "1.4.335.0"
VULKAN_SDK_EXE_PATH = "Engine/ThirdParty/VulkanSDK/VulkanSDK.exe"


def InstallVulkanSDK():
    print("Downloading {} to {}".format(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH))
    Utils.DownloadFile(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH)
    print("Done!")
    print("Running Vulkan SDK installer...")
    os.startfile(os.path.abspath(VULKAN_SDK_EXE_PATH))
    print("Re-run this script after installation")


def InstallVulkanPrompt():
    print("Would you like to install the Vulkan SDK?")
    install = Utils.YesOrNo()
    if install:
        InstallVulkanSDK()
        quit()


def CheckVulkanSDK():
    print("Checking for Vulkan SDK...")
    print(f"Detected VULKAN_SDK environment variable: {VULKAN_SDK}")
    print(f"Required Vulkan SDK version: {HAZEL_VULKAN_VERSION}")
    if VULKAN_SDK is None:
        print("You don't have the Vulkan SDK installed!")
        InstallVulkanPrompt()
        return False
    elif HAZEL_VULKAN_VERSION not in VULKAN_SDK:
        print(f"Located Vulkan SDK at {VULKAN_SDK}")
        print(
            f"You don't have the correct Vulkan SDK version! (Hazel requires {HAZEL_VULKAN_VERSION})"
        )
        InstallVulkanPrompt()
        return False

    print(f"Correct Vulkan SDK located at {VULKAN_SDK}")
    return True
