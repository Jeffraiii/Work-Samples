using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class ClickToLoad : MonoBehaviour {

	public void loadSceneByIndex (int index)
    {
        AkSoundEngine.StopAll();
        SceneManager.LoadScene(index);
    }
}
