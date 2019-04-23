using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class ClickToStart : MonoBehaviour {

    public void sceneLoad(string sceneNameString)
    {
        SceneManager.LoadScene(sceneNameString);
        //SceneManager.SetActiveScene(SceneManager.GetSceneByName(sceneNameString));
    }
}
