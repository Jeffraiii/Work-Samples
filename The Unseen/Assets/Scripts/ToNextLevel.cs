using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class ToNextLevel : MonoBehaviour {

    public int currentSceneIndex;

#if UNITY_EDITOR
    void Update () {
		if (Input.GetKeyDown(KeyCode.R))
        {
            AkSoundEngine.StopAll();
            SceneManager.LoadScene(currentSceneIndex);
        }
        if (Input.GetKeyDown(KeyCode.N))
        {
            AkSoundEngine.StopAll();
            SceneManager.LoadScene(currentSceneIndex + 1);
        }
        if (Input.GetKeyDown(KeyCode.B))
        {
            AkSoundEngine.StopAll();
            SceneManager.LoadScene(currentSceneIndex - 1);
        }

        if (Input.GetKeyDown(KeyCode.M))
        {
            AkSoundEngine.StopAll();
            SceneManager.LoadScene(0);
        }
    }
#endif
    void OnTriggerEnter(Collider playerCollider)
    {
        print("Destination Reached");
        AkSoundEngine.StopAll();
        SceneManager.LoadScene(currentSceneIndex + 1);
    }
}
