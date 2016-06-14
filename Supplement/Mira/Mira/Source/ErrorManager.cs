﻿using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace NovelEx {
	public class ErrorManager {
		private List<string> errorMessage = new List<string>();
		private List<string> warningMessage = new List<string>();

		public void Clear() {
			errorMessage.Clear();
			warningMessage.Clear();
		}

		public void addLog(string message, int line, bool stop) {
			if (stop) {
				string str = "<color=green>Novel</color>[" + JOKEREX.Instance.StatusManager.currentScenario + "]:<color=red>Error:" + line + "行目 " + message + "</color>";
				errorMessage.Add(str);
			}
			else {
				string str = "<color=green>Novel</color>[" + JOKEREX.Instance.StatusManager.currentScenario + "]:<color=yellow>Warning:" + line + "行目 " + message + "</color>";
				warningMessage.Add(str);
			}
		}

		public bool showAll() {
			foreach (string message in errorMessage) {
				Debug.LogError(message);
			}
			foreach (string message in warningMessage) {
				Debug.LogWarning(message);
			}
			return errorMessage.Count > 0 ? true : false;
		}

		public void stopError(string message) {
			Debug.LogError(message);
			throw new UnityException(message);		
		}
	};
}
