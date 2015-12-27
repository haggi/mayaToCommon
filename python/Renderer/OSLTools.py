import path
import logging
import subprocess
import pymel.core as pm
import os
import shutil
import sys
import xml.etree.cElementTree as ET
#from test.badsyntax_future3 import result

log = logging.getLogger("renderLogger")

def compileOSLShaders(renderer="Corona"):
    oslShaderPath = path.path("H:/UserDatenHaggi/Documents/coding/OpenMaya/src/mayaTo{0}/mt{1}_devmodule/shaders".format(renderer, renderer[:2].lower()))
    oslShadersToCompile = []
    for p in oslShaderPath.listdir("*.osl"):
        oslFile = path.path(p)
        osoFile = path.path(p[:-3] + "oso")
        if osoFile.exists():
            if osoFile.mtime < oslFile.mtime:
                osoFile.remove()
        if osoFile.exists():
            continue
        oslShadersToCompile.append(oslFile)
        
    for oslShader in oslShadersToCompile:
        cmd = "{compiler} -v -o {output} {oslFile}".format(compiler="oslc.exe", output=oslShader.replace(".osl", ".oso"), oslFile=oslShader)
        log.debug("Compiling osl shader: {0}".format(oslShader))
        log.debug("Command: {0}".format(cmd))
        
        IDLE_PRIORITY_CLASS = 64
        process = subprocess.Popen(cmd, bufsize=1, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, creationflags=IDLE_PRIORITY_CLASS)

        while 1:
            line = process.stdout.readline()
            if not line: break
            log.debug(line)
            pm.mel.trace(line.strip())

def getShaderInfo(shaderPath):
    print "Getting shader info for path", shaderPath    
    osoFiles = getOSOFiles(shaderPath)
    return osoFiles

def getOSODirs(renderer = "appleseed"):    
    try:
        shaderDir = os.environ['{0}_OSL_SHADERS_LOCATION'.format(renderer.upper())]
    except KeyError:
        shaderDir = path.path(__file__).parent() + "/shaders"
        print "Error: there is no environmentvariable called OSL_SHADERS_LOCATION. Please create one and point it to the base shader dir."
    osoDirs = set()
    for root, dirname, files in os.walk(shaderDir):
        for filename in files:
            if filename.endswith(".oso"):
                osoDirs.add(root.replace("\\", "/"))
    return list(osoDirs)

def getOSOFiles(renderer = "appleseed"):
    try:
        shaderDir = os.environ['{0}_OSL_SHADERS_LOCATION'.format(renderer.upper())]
    except KeyError:
        shaderDir = path.path(__file__).parent() + "/shaders"
        print "Error: there is no environmentvariable called OSL_SHADERS_LOCATION. Please create one and point it to the base shader dir."

    osoFiles = set()
    for root, dirname, files in os.walk(shaderDir):
        for filename in files:
            if filename.endswith(".oso"):
                osoFiles.add(os.path.join(root, filename).replace("\\", "/"))
    return list(osoFiles)

def getOSLFiles(renderer = "appleseed"):
    try:
        shaderDir = os.environ['{0}_OSL_SHADERS_LOCATION'.format(renderer.upper())]
    except KeyError:
        shaderDir = path.path(__file__).parent() + "/shaders"
        print "Error: there is no environmentvariable called OSL_SHADERS_LOCATION. Please create one and point it to the base shader dir."

    osoFiles = set()
    for root, dirname, files in os.walk(shaderDir):
        for filename in files:
            if filename.endswith(".osl"):
                osoFiles.add(os.path.join(root, filename).replace("\\", "/"))
    return list(osoFiles)

import pprint

def analyzeContent(content):
    print "Analyze Content"
    r = content
    d = {}
    currentElement = None
    for line in content:
        if len(line) == 0:
            continue
        if line.startswith("shader"):
            d['name'] = line.split(" ")[1].replace("\"", "")
            d['mayaClassification'] = ""
            d['mayaId'] = 0
            d['help'] = ""
            d['inputs'] = []
            d['outputs'] = []
            currentElement = d
        else:
            if line.startswith("Default value"):
                currentElement['default'] = line.split(" ")[-1].replace("\"", "")
                if currentElement.has_key("type"):
                    if currentElement["type"] in ["color", "vector"]:
                        vector = line.split("[")[-1].split("]")[0]
                        vector = vector.strip()
                        currentElement['default'] = map(float, vector.split(" "))
            if line.startswith("metadata"):
                if "min = " in line:
                    currentElement['min'] = line.split(" ")[-1]
                if "max = " in line:
                    currentElement['max'] = line.split(" ")[-1]
                if "help = " in line:
                    currentElement['help'] = " ".join(line.split("=")[1:]).replace("\"", "").strip()
                if "mayaClassification = " in line:
                    print "mayaClassification", " ".join(line.split("=")[1:]).replace("\"", "").strip()
                    currentElement['mayaClassification'] = " ".join(line.split("=")[1:]).replace("\"", "").strip()
                if "mayaId = " in line:
                    print "Found maya id", int(line.split("=")[-1])
                    currentElement['mayaId'] = int(line.split("=")[-1])
            if line.startswith("\""): # found a parameter
                currentElement = {}
                currentElement['name'] = line.split(" ")[0].replace("\"", "")
                currentElement['type'] = " ".join(line.split(" ")[1:]).replace("\"", "")
                if "output" in line:
                    d['outputs'].append(currentElement)
                    currentElement = d['outputs'][-1]
                else:
                    d['inputs'].append(currentElement)
                    currentElement = d['inputs'][-1]
    return d

def readShadersXMLDescription():
    if "MayaToCommon" in path.path(__file__):
        xmlFile = path.path("H:/UserDatenHaggi/Documents/coding/mayaToAppleseed/mtap_devmodule/resources/shaderDefinitions.xml")
    else:
        xmlFile = path.path(__file__).parent / "resources/shaderDefinitions.xml"
    if not xmlFile.exists():
        log.error("No shader xml file: {0}".format(xmlFile))
        return
    tree = ET.parse(xmlFile)
    shaders = tree.getroot()
    shaderList = []
    for shader in shaders:
        shDict = {}
        shDict['name'] = shader.find('name').text
        shDict['classification'] = ""
        shDict['mayaId'] = 0
        shDict['help'] = ""
        shDict['inputs'] = []
        shDict['outputs'] = []
        for inp in shader.find('inputs'):
            inpp = {}
            inpp['name'] = inp.find('name').text
            inpp['type'] = inp.find('type').text
            inpp['help'] = ""
            inpp['min'] = 0
            inpp['max'] = 1
            inpp['default'] = 0
            if inp.find('min'):
                inpp['min'] = inp.find('min').text
            if inp.find('max'):
                inpp['max'] = inp.find('max').text
            if inp.find('default'):
                inpp['default'] = inp.find('default').text
            shDict['inputs'].append(inpp)
        for inp in shader.find('outputs'):
            inpp = {}
            inpp['name'] = inp.find('name').text
            inpp['type'] = inp.find('type').text
            inpp['help'] = ""
            shDict['outputs'].append(inpp)
        shaderList.append(shDict)
    return shaderList

def updateOSLShaderInfo(force=False, osoFiles=[]):
    pp = pprint.PrettyPrinter(indent=4)
    IDLE_PRIORITY_CLASS = 64
    cmd = "oslinfo -v"
    infoDict = {}
    for osoFile in osoFiles:        
        infoCmd = cmd + " " + osoFile
        log.info("Info cmd: {0}".format(infoCmd))
        process = subprocess.Popen(infoCmd, bufsize=1, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, creationflags=IDLE_PRIORITY_CLASS)               
        content = []
        while 1:
            line = process.stdout.readline()
            line = line.strip()
            content.append(line)
            if not line: break
        
        infoDict[osoFile.split("/")[-1]] = analyzeContent(content)
    pp.pprint(infoDict)   
    
def compileAllShaders(renderer = "appleseed"):

    try:
        shaderDir = os.environ['{0}_OSL_SHADERS_LOCATION'.format(renderer.upper())]
    except KeyError:
        log.error("Error: there is no environmentvariable called OSL_SHADERS_LOCATION. Please create one and point it to the base shader dir.")
        # we expect this file in module/scripts so we can try to find the shaders in ../shaders
        log.error("Trying to find the shaders dir from current file: {0}".format(__file__))
        shaderDir = path.path(__file__).parent / "shaders"
        if shaderDir.exists():
            log.info("Using found shaders directory {0}".format(shaderDir))
            
    include_dir = os.path.join(shaderDir, "src/include")
    log.info("reading shaders from {0}".format(shaderDir))
    oslc_cmd = "oslc"
    failureDict = {}
    osoInfoShaders = []
    
    for root, dirname, files in os.walk(shaderDir):
        for filename in files:
            if filename.endswith(".osl"):
                oslPath =  os.path.join(root, filename)
                dest_dir = root.replace("\\", "/").replace("shaders/src", "shaders") + "/"
                if not os.path.exists(dest_dir):
                    os.makedirs(dest_dir)
                osoOutputPath = dest_dir + filename.replace(".osl", ".oso")                
                osoOutputFile = path.path(osoOutputPath)
                oslInputFile = path.path(oslPath)
                
                if osoOutputFile.exists():
                    if osoOutputFile.mtime > oslInputFile.mtime:
                        log.debug("oso file {0} up to date, no compilation needed.".format(osoOutputFile.basename()))
                        continue
                    else:
                        osoOutputFile.remove()

                log.debug("compiling shader: {0}".format(oslInputFile))
    
                saved_wd = os.getcwd()
                os.chdir(root)
                compileCmd = oslc_cmd + " -v -I" + include_dir + ' -o '+ osoOutputPath + ' ' + oslInputFile
                log.debug("compile command: {0}".format(compileCmd))
                
                IDLE_PRIORITY_CLASS = 64
                process = subprocess.Popen(compileCmd, bufsize=1, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, creationflags=IDLE_PRIORITY_CLASS)
                
                progress = []
                fail = False
                while 1:
                    line = process.stdout.readline()
                    if not line: break
                    log.debug(line)
                    line.strip()
                    progress.append(line)
                    pm.mel.trace(line.strip())
                    if "error" in line:
                        fail = True
                if fail:
                    print "set dict", osoOutputFile.basename(), "to", progress
                    failureDict[osoOutputFile.basename()] = progress
                else:
                    osoInfoShaders.append(osoOutputPath)
                os.chdir(saved_wd)
    
    if len(failureDict.keys()) > 0:
        log.info("\n\nShader compilation failed for:")
        for key, content in failureDict.iteritems():
            log.info("Shader {0}\n{1}\n\n".format(key, "\n".join(content)))
    else:
        log.info("Shader compilation done.")
        if len(osoInfoShaders) > 0:
            log.info("Updating shaderInfoFile.")
            updateOSLShaderInfo(force=False, osoFiles=osoInfoShaders)
            

